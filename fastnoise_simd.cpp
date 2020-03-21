/*************************************************************************/
/*  fast_noise_simd.cpp                                                  */
/*************************************************************************/
/* Copyright (c) 2020 Cory Petkovsek                                     */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "fastnoise_simd.h"

FastNoiseSIMD::FastNoiseSIMD() {

	// Create a new FastNoiseSIMD for the highest supported instuction set of the CPU
	_noise = _FastNoiseSIMD::NewFastNoiseSIMD();

	// Most defaults copied from the library
	set_noise_type(TYPE_VALUE);
	set_seed(0);
	set_frequency(0.01);
	set_offset(Vector3(0.0, 0.0, 0.0));
	set_axis_scales(Vector3(1.0, 1.0, 1.0));
	set_scale(1.0);

	set_fractal_type(FRACTAL_FBM);
	set_fractal_octaves(3);
	set_fractal_lacunarity(2.0);
	set_fractal_gain(0.5);

	set_cellular_distance_function(DISTANCE_EUCLIDEAN);
	set_cellular_return_type(RETURN_CELL_VALUE);
	set_cellular_distance2_indices(0, 1);
	set_cellular_jitter(0.45);

	set_perturb_type(PERTURB_NONE);
	set_perturb_amplitude(1);
	set_perturb_frequency(0.5);
	set_perturb_fractal_octaves(3);
	set_perturb_fractal_lacunarity(2.0);
	set_perturb_fractal_gain(0.5);
	set_perturb_normalize_length(1.0);
}

FastNoiseSIMD::~FastNoiseSIMD() {
	while (_allocated_sets.front()) {
		_noise->FreeNoiseSet(_allocated_sets.front()->get());
		_allocated_sets.pop_front();
	}
}

// General settings

void FastNoiseSIMD::set_noise_type(NoiseType p_noise_type) {
	_noise->SetNoiseType((_FastNoiseSIMD::NoiseType)p_noise_type);
	emit_changed();
}

FastNoiseSIMD::NoiseType FastNoiseSIMD::get_noise_type() const {
	return (NoiseType)_noise->GetNoiseType();
}

void FastNoiseSIMD::set_seed(int p_seed) {
	_noise->SetSeed(p_seed);
	emit_changed();
}

int FastNoiseSIMD::get_seed() const {
	return _noise->GetSeed();
}

void FastNoiseSIMD::set_frequency(float p_freq) {
	_noise->SetFrequency(p_freq);
	emit_changed();
}

float FastNoiseSIMD::get_frequency() const {
	return _noise->GetFrequency();
}

void FastNoiseSIMD::set_offset(Vector3 p_offset) {
	_offset = p_offset;
	emit_changed();
}

Vector3 FastNoiseSIMD::get_offset() const {
	return _offset;
}

void FastNoiseSIMD::set_axis_scales(Vector3 p_scale) {
	_axis_scales.x = p_scale.x;
	_axis_scales.y = p_scale.y;
	_axis_scales.z = p_scale.z;
	_noise->SetAxisScales(p_scale.x, p_scale.y, p_scale.z);
	emit_changed();
}

Vector3 FastNoiseSIMD::get_axis_scales() const {
	return _axis_scales;
}

void FastNoiseSIMD::set_scale(float p_scale) {
	_scale = p_scale;
	emit_changed();
}

float FastNoiseSIMD::get_scale() const {
	return _scale;
}

void FastNoiseSIMD::set_simd_level(int p_level) {
	_noise->SetSIMDLevel(p_level);
	emit_changed();
}

int FastNoiseSIMD::get_simd_level() const {
	return _noise->GetSIMDLevel();
}

// Noise singular value functions

float FastNoiseSIMD::get_noise_1d(float p_z) {
	return get_noise_3d(0.0, 0.0, p_z);
}

float FastNoiseSIMD::get_noise_2dv(Vector2 p_v) {
	return get_noise_3d(p_v.x, 0.0, p_v.y);
}

float FastNoiseSIMD::get_noise_2d(float p_x, float p_z) {
	return get_noise_3d(p_x, 0.0, p_z);
}

float FastNoiseSIMD::get_noise_3dv(Vector3 p_v) {
	return get_noise_3d(p_v.x, p_v.y, p_v.z);
}

float FastNoiseSIMD::get_noise_3d(float p_x, float p_y, float p_z) {
	float *n = _noise->GetNoiseSet(p_x + _offset.x, p_y + _offset.y, p_z + _offset.z, 1.0, 1.0, 1.0);
	float v = *n;
	_noise->FreeNoiseSet(n);
	return v;
}

// Noise set functions

float *FastNoiseSIMD::get_noise_set_1d(float p_z, int p_sizez, float p_scale) {
	return get_noise_set_3d(0.0, 0.0, p_z, 1.0, 1.0, p_sizez, p_scale);
}

float *FastNoiseSIMD::get_noise_set_2dv(Vector2 p_v, Vector2 p_size, float p_scale) {
	return get_noise_set_3d(p_v.x, 0.0, p_v.y, p_size.x, 1.0, p_size.y, p_scale);
}

float *FastNoiseSIMD::get_noise_set_2d(float p_x, float p_z, int p_sizex, int p_sizez, float p_scale) {
	return get_noise_set_3d(p_x, 0.0, p_z, p_sizex, 1.0, p_sizez, p_scale);
}

float *FastNoiseSIMD::get_noise_set_3dv(Vector3 p_v, Vector3 p_size, float p_scale) {
	return get_noise_set_3d(p_v.x, p_v.y, p_v.z, p_size.x, p_size.y, p_size.z, p_scale);
}

float *FastNoiseSIMD::get_noise_set_3d(int p_x, int p_y, int p_z, int p_sizex, int p_sizey, int p_sizez, float p_scale) {
	float *ns;
	float scale = (p_scale == 0.0) ? _scale : p_scale;
	ns = _noise->GetNoiseSet(p_x + _offset.x, p_y + _offset.y, p_z + _offset.z, p_sizex, p_sizey, p_sizez, scale);
	_allocated_sets.push_back(ns);
	return ns;
}

float *FastNoiseSIMD::get_empty_set_3dv(Vector3 p_size) {
	return get_empty_set(p_size.x * p_size.y * p_size.z);
}

float *FastNoiseSIMD::get_empty_set(int p_size) {
	float *set = _noise->GetEmptySet(p_size);
	_allocated_sets.push_back(set);
	return set;
}

void FastNoiseSIMD::fill_noise_set_3dv(float *p_set, Vector3 p_v, Vector3 p_size, float p_scale) {
	fill_noise_set_3d(p_set, p_v.x, p_v.y, p_v.z, p_size.x, p_size.y, p_size.z, p_scale);
}

void FastNoiseSIMD::fill_noise_set_3d(float *p_set, int p_x, int p_y, int p_z, int p_sizex, int p_sizey, int p_sizez, float p_scale) {
	float scale = (p_scale == 0.0) ? _scale : p_scale;
	_noise->FillNoiseSet(p_set, p_x + _offset.x, p_y + _offset.y, p_z + _offset.z, p_sizex, p_sizey, p_sizez, scale);
	return;
}

void FastNoiseSIMD::free_noise_set(float *p_set) {
	_allocated_sets.erase(p_set);
	_noise->FreeNoiseSet(p_set);
}

// Allocate PoolVectors for GDScript

Vector<float> FastNoiseSIMD::_b_get_noise_set_1d(float p_z, int p_sizez, float p_scale) {
	float scale = (p_scale == 0.0) ? _scale : p_scale;
	float *ns = _noise->GetNoiseSet(0.f, 0.f, p_z + _offset.z, 1.f, 1.f, p_sizez, scale);

	Vector<float> pv;
	pv.resize(p_sizez);
	float *wr = pv.ptrw();
	memcpy(wr, ns, p_sizez * sizeof(float));

	_noise->FreeNoiseSet(ns);
	return pv;
}

Vector<float> FastNoiseSIMD::_b_get_noise_set_2dv(Vector2 p_v, Vector2 p_size, float p_scale) {
	float scale = (p_scale == 0.0) ? _scale : p_scale;
	float *ns = _noise->GetNoiseSet(p_v.x + _offset.x, 0.f, p_v.y + _offset.z, p_size.x, 1.f, p_size.y, scale);

	Vector<float> pv;
	int size = p_size.x * p_size.y;
	pv.resize(size);
	float *wr = pv.ptrw();
	memcpy(wr, ns, size * sizeof(float));

	_noise->FreeNoiseSet(ns);
	return pv;
}

Vector<float> FastNoiseSIMD::_b_get_noise_set_3dv(Vector3 p_v, Vector3 p_size, float p_scale) {
	float scale = (p_scale == 0.0) ? _scale : p_scale;
	float *ns = _noise->GetNoiseSet(p_v.x + _offset.x, p_v.y + _offset.y, p_v.z + _offset.z, p_size.x, p_size.y, p_size.z, scale);

	Vector<float> pv;
	int size = p_size.x * p_size.y * p_size.z;
	pv.resize(size);
	float *wr = pv.ptrw();
	memcpy(wr, ns, size * sizeof(float));

	_noise->FreeNoiseSet(ns);
	return pv;
}

// Perturb

void FastNoiseSIMD::set_perturb_type(PerturbType p_type) {
	_noise->SetPerturbType((_FastNoiseSIMD::PerturbType)p_type);
	emit_changed();
}

FastNoiseSIMD::PerturbType FastNoiseSIMD::get_perturb_type() const {
	return (PerturbType)_noise->GetPerturbType();
}

void FastNoiseSIMD::set_perturb_amplitude(float p_amp) {
	_perturb_amp = p_amp;
	_noise->SetPerturbAmp(_perturb_amp);
	emit_changed();
}

float FastNoiseSIMD::get_perturb_amplitude() const {
	return _perturb_amp;
}

void FastNoiseSIMD::set_perturb_frequency(float p_freq) {
	_noise->SetPerturbFrequency(p_freq);
	emit_changed();
}

float FastNoiseSIMD::get_perturb_frequency() const {
	return _noise->GetPerturbFrequency();
}

void FastNoiseSIMD::set_perturb_fractal_octaves(int p_octaves) {
	_noise->SetPerturbFractalOctaves(p_octaves);
	emit_changed();
}

int FastNoiseSIMD::get_perturb_fractal_octaves() {
	return _noise->GetPerturbFractalOctaves();
}

void FastNoiseSIMD::set_perturb_fractal_lacunarity(float p_lacunarity) {
	_noise->SetPerturbFractalLacunarity(p_lacunarity);
	emit_changed();
}

float FastNoiseSIMD::get_perturb_fractal_lacunarity() {
	return _noise->GetPerturbFractalLacunarity();
}

void FastNoiseSIMD::set_perturb_fractal_gain(float p_gain) {
	_noise->SetPerturbFractalGain(p_gain);
	emit_changed();
}

float FastNoiseSIMD::get_perturb_fractal_gain() {
	return _noise->GetPerturbFractalGain();
}

void FastNoiseSIMD::set_perturb_normalize_length(float p_length) {
	_noise->SetPerturbNormaliseLength(p_length);
	emit_changed();
}

float FastNoiseSIMD::get_perturb_normalize_length() {
	return _noise->GetPerturbNormaliseLength();
}

// Fractal

void FastNoiseSIMD::set_fractal_type(FractalType p_type) {
	_noise->SetFractalType((_FastNoiseSIMD::FractalType)p_type);
	emit_changed();
}

FastNoiseSIMD::FractalType FastNoiseSIMD::get_fractal_type() const {
	return (FractalType)_noise->GetFractalType();
}

void FastNoiseSIMD::set_fractal_octaves(int p_octaves) {
	_noise->SetFractalOctaves(p_octaves);
	emit_changed();
}

int FastNoiseSIMD::get_fractal_octaves() const {
	return _noise->GetFractalOctaves();
}

void FastNoiseSIMD::set_fractal_lacunarity(float p_lacunarity) {
	_noise->SetFractalLacunarity(p_lacunarity);
	emit_changed();
}

float FastNoiseSIMD::get_fractal_lacunarity() const {
	return _noise->GetFractalLacunarity();
}

void FastNoiseSIMD::set_fractal_gain(float p_gain) {
	_noise->SetFractalGain(p_gain);
	emit_changed();
}

float FastNoiseSIMD::get_fractal_gain() const {
	return _noise->GetFractalGain();
}

// Cellular

void FastNoiseSIMD::set_cellular_distance_function(CellularDistanceFunction p_func) {
	_noise->SetCellularDistanceFunction((_FastNoiseSIMD::CellularDistanceFunction)p_func);
	emit_changed();
}

FastNoiseSIMD::CellularDistanceFunction FastNoiseSIMD::get_cellular_distance_function() const {
	return (CellularDistanceFunction)_noise->GetCellularDistanceFunction();
}

void FastNoiseSIMD::set_cellular_return_type(CellularReturnType p_type) {
	_noise->SetCellularReturnType((_FastNoiseSIMD::CellularReturnType)p_type);
	emit_changed();
}

FastNoiseSIMD::CellularReturnType FastNoiseSIMD::get_cellular_return_type() const {
	return (CellularReturnType)_noise->GetCellularReturnType();
}

void FastNoiseSIMD::set_cellular_distance2_indices(int p_index0, int p_index1) {
	// Valid range for index1: 1-3
	if (p_index1 > 3) {
		_cell_dist_index1 = 3;
	} else if (p_index1 < 1) {
		_cell_dist_index1 = 1;
	} else {
		_cell_dist_index1 = p_index1;
	}

	// Valid range for index0: 0-2 and < index1
	_cell_dist_index0 = p_index0;
	if (_cell_dist_index0 >= _cell_dist_index1) {
		_cell_dist_index0 = _cell_dist_index1 - 1;
	}

	if (_cell_dist_index0 < 0) {
		_cell_dist_index0 = 0;
	}

	_noise->SetCellularDistance2Indices(_cell_dist_index0, _cell_dist_index1);
	emit_changed();
}

PackedInt32Array FastNoiseSIMD::get_cellular_distance2_indices() const {
	PackedInt32Array a;
	a.append(_cell_dist_index0);
	a.append(_cell_dist_index1);
	return a;
}

void FastNoiseSIMD::set_cellular_distance2_index0(int p_index0) {
	set_cellular_distance2_indices(p_index0, _cell_dist_index1);
	emit_changed();
}

int FastNoiseSIMD::get_cellular_distance2_index0() const {
	return _cell_dist_index0;
}

void FastNoiseSIMD::set_cellular_distance2_index1(int p_index1) {
	set_cellular_distance2_indices(_cell_dist_index0, p_index1);
	emit_changed();
}

int FastNoiseSIMD::get_cellular_distance2_index1() const {
	return _cell_dist_index1;
}

void FastNoiseSIMD::set_cellular_jitter(float p_jitter) {
	_noise->SetCellularJitter(p_jitter);
	emit_changed();
}

float FastNoiseSIMD::get_cellular_jitter() const {
	return _noise->GetCellularJitter();
}

void FastNoiseSIMD::set_cellular_noise_lookup_type(NoiseType p_type) {
	_noise->SetCellularNoiseLookupType((_FastNoiseSIMD::NoiseType)p_type);
	emit_changed();
}

FastNoiseSIMD::NoiseType FastNoiseSIMD::get_cellular_noise_lookup_type() const {
	return (NoiseType)_noise->GetCellularNoiseLookupType();
}

void FastNoiseSIMD::set_cellular_noise_lookup_frequency(float p_freq) {
	_noise->SetCellularNoiseLookupFrequency(p_freq);
	emit_changed();
}

float FastNoiseSIMD::get_cellular_noise_lookup_frequency() const {
	return _noise->GetCellularNoiseLookupFrequency();
}

// Generate Textures

Ref<Image> FastNoiseSIMD::get_image(int p_width, int p_height, bool p_invert) {

	Vector<uint8_t> data;
	data.resize(p_width * p_height * 4);

	uint8_t *wd8 = data.ptrw();

	float *noise_set = get_noise_set_2d(0.0, 0.0, p_height, p_width);

	// Get all values and identify min/max values
	float min_val = 100;
	float max_val = -100;

	for (int i = 0; i < p_width * p_height; i++) {
		if (noise_set[i] > max_val) {
			max_val = noise_set[i];
		}
		if (noise_set[i] < min_val) {
			min_val = noise_set[i];
		}
	}

	// Normalize values and write to texture
	if (max_val != min_val) {
		for (int x = 0; x < p_width * p_height; x++) {
			uint8_t value = uint8_t((noise_set[x] - min_val) / (max_val - min_val) * 255.f);
			if (p_invert) {
				value = 255 - value;
			}
			wd8[x * 4 + 0] = value;
			wd8[x * 4 + 1] = value;
			wd8[x * 4 + 2] = value;
			wd8[x * 4 + 3] = 255;
		}
	}

	free_noise_set(noise_set);

	Ref<Image> image = memnew(Image(p_width, p_height, false, Image::FORMAT_RGBA8, data));
	return image;
}

Ref<Image> FastNoiseSIMD::get_seamless_image(int p_width, int p_height, bool p_invert) {
	return Noise::get_seamless_image(p_width, p_height, p_invert);
}

void FastNoiseSIMD::_bind_methods() {

	// General settings

	ClassDB::bind_method(D_METHOD("set_noise_type", "type"), &FastNoiseSIMD::set_noise_type);
	ClassDB::bind_method(D_METHOD("get_noise_type"), &FastNoiseSIMD::get_noise_type);
#ifdef ENABLE_SIMPLEX
	ADD_PROPERTY(PropertyInfo(Variant::INT, "noise_type", PROPERTY_HINT_ENUM,
						 "Value,ValueFractal,Perlin,PerlinFractal,Simplex,SimplexFractal,WhiteNoise,Cellular,Cubic,CubicFractal"),
			"set_noise_type", "get_noise_type");
#else
	ADD_PROPERTY(PropertyInfo(Variant::INT, "noise_type", PROPERTY_HINT_ENUM,
						 "Value,ValueFractal,Perlin,PerlinFractal,WhiteNoise,Cellular,Cubic,CubicFractal"),
			"set_noise_type", "get_noise_type");
#endif

	ClassDB::bind_method(D_METHOD("set_seed", "seed"), &FastNoiseSIMD::set_seed);
	ClassDB::bind_method(D_METHOD("get_seed"), &FastNoiseSIMD::get_seed);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

	ClassDB::bind_method(D_METHOD("set_frequency", "freq"), &FastNoiseSIMD::set_frequency);
	ClassDB::bind_method(D_METHOD("get_frequency"), &FastNoiseSIMD::get_frequency);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency", PROPERTY_HINT_RANGE, "0.001,1"), "set_frequency", "get_frequency");

	ClassDB::bind_method(D_METHOD("set_offset", "offset"), &FastNoiseSIMD::set_offset);
	ClassDB::bind_method(D_METHOD("get_offset"), &FastNoiseSIMD::get_offset);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "offset", PROPERTY_HINT_RANGE, "-999999999,999999999,1"), "set_offset", "get_offset");

	ClassDB::bind_method(D_METHOD("set_axis_scales", "axis_scales"), &FastNoiseSIMD::set_axis_scales);
	ClassDB::bind_method(D_METHOD("get_axis_scales"), &FastNoiseSIMD::get_axis_scales);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "axis_scales"), "set_axis_scales", "get_axis_scales");

	ClassDB::bind_method(D_METHOD("set_scale", "scale"), &FastNoiseSIMD::set_scale);
	ClassDB::bind_method(D_METHOD("get_scale"), &FastNoiseSIMD::get_scale);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "scale"), "set_scale", "get_scale");

	ClassDB::bind_method(D_METHOD("set_simd_level", "simd_level"), &FastNoiseSIMD::set_simd_level);
	ClassDB::bind_method(D_METHOD("get_simd_level"), &FastNoiseSIMD::get_simd_level);

	// Noise functions

	ClassDB::bind_method(D_METHOD("get_noise_1d", "z"), &FastNoiseSIMD::get_noise_1d);
	ClassDB::bind_method(D_METHOD("get_noise_2dv", "v"), &FastNoiseSIMD::get_noise_2dv);
	ClassDB::bind_method(D_METHOD("get_noise_2d", "x", "z"), &FastNoiseSIMD::get_noise_2d);
	ClassDB::bind_method(D_METHOD("get_noise_3dv", "v"), &FastNoiseSIMD::get_noise_3dv);
	ClassDB::bind_method(D_METHOD("get_noise_3d", "x", "y", "z"), &FastNoiseSIMD::get_noise_3d);

	ClassDB::bind_method(D_METHOD("get_noise_set_1d", "z", "size", "scale"), &FastNoiseSIMD::_b_get_noise_set_1d, DEFVAL(0.0f));
	ClassDB::bind_method(D_METHOD("get_noise_set_2dv", "v", "size", "scale"), &FastNoiseSIMD::_b_get_noise_set_2dv, DEFVAL(0.0f));
	ClassDB::bind_method(D_METHOD("get_noise_set_3dv", "v", "size", "scale"), &FastNoiseSIMD::_b_get_noise_set_3dv, DEFVAL(0.0f));

	// Perturb

	ClassDB::bind_method(D_METHOD("set_perturb_type", "type"), &FastNoiseSIMD::set_perturb_type);
	ClassDB::bind_method(D_METHOD("get_perturb_type"), &FastNoiseSIMD::get_perturb_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "perturb_type", PROPERTY_HINT_ENUM, "None,Gradient,Gradient_Fractal,Normalize,Gradient_Normalize,Gradient_Fractal_Normalize"), "set_perturb_type", "get_perturb_type");

	ClassDB::bind_method(D_METHOD("set_perturb_amplitude", "amp"), &FastNoiseSIMD::set_perturb_amplitude);
	ClassDB::bind_method(D_METHOD("get_perturb_amplitude"), &FastNoiseSIMD::get_perturb_amplitude);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "perturb_amplitude"), "set_perturb_amplitude", "get_perturb_amplitude");

	ClassDB::bind_method(D_METHOD("set_perturb_frequency", "freq"), &FastNoiseSIMD::set_perturb_frequency);
	ClassDB::bind_method(D_METHOD("get_perturb_frequency"), &FastNoiseSIMD::get_perturb_frequency);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "perturb_frequency"), "set_perturb_frequency", "get_perturb_frequency");

	ClassDB::bind_method(D_METHOD("set_perturb_fractal_octaves", "octaves"), &FastNoiseSIMD::set_perturb_fractal_octaves);
	ClassDB::bind_method(D_METHOD("get_perturb_fractal_octaves"), &FastNoiseSIMD::get_perturb_fractal_octaves);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "perturb_fractal_octaves", PROPERTY_HINT_RANGE, "1,10,1"), "set_perturb_fractal_octaves", "get_perturb_fractal_octaves");

	ClassDB::bind_method(D_METHOD("set_perturb_fractal_lacunarity", "lacunarity"), &FastNoiseSIMD::set_perturb_fractal_lacunarity);
	ClassDB::bind_method(D_METHOD("get_perturb_fractal_lacunarity"), &FastNoiseSIMD::get_perturb_fractal_lacunarity);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "perturb_fractal_lacunarity"), "set_perturb_fractal_lacunarity", "get_perturb_fractal_lacunarity");

	ClassDB::bind_method(D_METHOD("set_perturb_fractal_gain", "gain"), &FastNoiseSIMD::set_perturb_fractal_gain);
	ClassDB::bind_method(D_METHOD("get_perturb_fractal_gain"), &FastNoiseSIMD::get_perturb_fractal_gain);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "perturb_fractal_gain"), "set_perturb_fractal_gain", "get_perturb_fractal_gain");

	ClassDB::bind_method(D_METHOD("set_perturb_normalize_length", "length"), &FastNoiseSIMD::set_perturb_normalize_length);
	ClassDB::bind_method(D_METHOD("get_perturb_normalize_length"), &FastNoiseSIMD::get_perturb_normalize_length);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "perturb_normalize_length"), "set_perturb_normalize_length", "get_perturb_normalize_length");

	// Fractal

	ClassDB::bind_method(D_METHOD("set_fractal_type", "type"), &FastNoiseSIMD::set_fractal_type);
	ClassDB::bind_method(D_METHOD("get_fractal_type"), &FastNoiseSIMD::get_fractal_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fractal_type", PROPERTY_HINT_ENUM, "FBM,Billow,RidgedMulti"), "set_fractal_type", "get_fractal_type");

	ClassDB::bind_method(D_METHOD("set_fractal_octaves", "octaves"), &FastNoiseSIMD::set_fractal_octaves);
	ClassDB::bind_method(D_METHOD("get_fractal_octaves"), &FastNoiseSIMD::get_fractal_octaves);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fractal_octaves", PROPERTY_HINT_RANGE, "1,10,1"), "set_fractal_octaves", "get_fractal_octaves");

	ClassDB::bind_method(D_METHOD("set_fractal_lacunarity", "lacunarity"), &FastNoiseSIMD::set_fractal_lacunarity);
	ClassDB::bind_method(D_METHOD("get_fractal_lacunarity"), &FastNoiseSIMD::get_fractal_lacunarity);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fractal_lacunarity"), "set_fractal_lacunarity", "get_fractal_lacunarity");

	ClassDB::bind_method(D_METHOD("set_fractal_gain", "gain"), &FastNoiseSIMD::set_fractal_gain);
	ClassDB::bind_method(D_METHOD("get_fractal_gain"), &FastNoiseSIMD::get_fractal_gain);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fractal_gain"), "set_fractal_gain", "get_fractal_gain");

	// Cellular

	ClassDB::bind_method(D_METHOD("set_cellular_distance_function", "func"), &FastNoiseSIMD::set_cellular_distance_function);
	ClassDB::bind_method(D_METHOD("get_cellular_distance_function"), &FastNoiseSIMD::get_cellular_distance_function);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cellular_dist_func", PROPERTY_HINT_ENUM, "Euclidean,Manhattan,Natural"), "set_cellular_distance_function", "get_cellular_distance_function");

	ClassDB::bind_method(D_METHOD("set_cellular_jitter", "jitter"), &FastNoiseSIMD::set_cellular_jitter);
	ClassDB::bind_method(D_METHOD("get_cellular_jitter"), &FastNoiseSIMD::get_cellular_jitter);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cellular_jitter"), "set_cellular_jitter", "get_cellular_jitter");

	ClassDB::bind_method(D_METHOD("set_cellular_return_type", "type"), &FastNoiseSIMD::set_cellular_return_type);
	ClassDB::bind_method(D_METHOD("get_cellular_return_type"), &FastNoiseSIMD::get_cellular_return_type);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cellular_return_type", PROPERTY_HINT_ENUM, "CellValue,Distance,Distance2,Distance2Add,Distance2Sub,Distance2Mul,Distance2Div,Distance2Cave,NoiseLookup"), "set_cellular_return_type", "get_cellular_return_type");

	ClassDB::bind_method(D_METHOD("set_cellular_distance2_indices", "index0", "index1"), &FastNoiseSIMD::set_cellular_distance2_indices);
	ClassDB::bind_method(D_METHOD("get_cellular_distance2_indices"), &FastNoiseSIMD::get_cellular_distance2_indices);
	ClassDB::bind_method(D_METHOD("set_cellular_distance2_index0", "index0"), &FastNoiseSIMD::set_cellular_distance2_index0);
	ClassDB::bind_method(D_METHOD("get_cellular_distance2_index0"), &FastNoiseSIMD::get_cellular_distance2_index0);
	ClassDB::bind_method(D_METHOD("set_cellular_distance2_index1", "index1"), &FastNoiseSIMD::set_cellular_distance2_index1);
	ClassDB::bind_method(D_METHOD("get_cellular_distance2_index1"), &FastNoiseSIMD::get_cellular_distance2_index1);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cellular_distance2_index0", PROPERTY_HINT_RANGE, "0,2,1"), "set_cellular_distance2_index0", "get_cellular_distance2_index0");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cellular_distance2_index1", PROPERTY_HINT_RANGE, "1,3,1"), "set_cellular_distance2_index1", "get_cellular_distance2_index1");

	ClassDB::bind_method(D_METHOD("set_cellular_noise_lookup_type", "type"), &FastNoiseSIMD::set_cellular_noise_lookup_type);
	ClassDB::bind_method(D_METHOD("get_cellular_noise_lookup_type"), &FastNoiseSIMD::get_cellular_noise_lookup_type);
#ifdef ENABLE_SIMPLEX
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cellular_noise_lookup_type", PROPERTY_HINT_ENUM,
						 "Value,ValueFractal,Perlin,PerlinFractal,Simplex,SimplexFractal,WhiteNoise,Cellular,Cubic,CubicFractal"),
			"set_cellular_noise_lookup_type", "get_cellular_noise_lookup_type");
#else
	ADD_PROPERTY(PropertyInfo(Variant::INT, "cellular_noise_lookup_type", PROPERTY_HINT_ENUM,
						 "Value,ValueFractal,Perlin,PerlinFractal,WhiteNoise,Cellular,Cubic,CubicFractal"),
			"set_cellular_noise_lookup_type", "get_cellular_noise_lookup_type");
#endif

	ClassDB::bind_method(D_METHOD("set_cellular_noise_lookup_frequency", "freq"), &FastNoiseSIMD::set_cellular_noise_lookup_frequency);
	ClassDB::bind_method(D_METHOD("get_cellular_noise_lookup_frequency"), &FastNoiseSIMD::get_cellular_noise_lookup_frequency);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cellular_noise_lookup_frequency"), "set_cellular_noise_lookup_frequency", "get_cellular_noise_lookup_frequency");

	// Textures
	ClassDB::bind_method(D_METHOD("get_image", "width", "height", "invert"), &FastNoiseSIMD::get_image, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_seamless_image", "width", "height", "invert"), &FastNoiseSIMD::get_seamless_image, DEFVAL(false));


	BIND_ENUM_CONSTANT(TYPE_VALUE);
	BIND_ENUM_CONSTANT(TYPE_VALUE_FRACTAL);
	BIND_ENUM_CONSTANT(TYPE_PERLIN);
	BIND_ENUM_CONSTANT(TYPE_PERLIN_FRACTAL);
#ifdef ENABLE_SIMPLEX
	BIND_ENUM_CONSTANT(TYPE_SIMPLEX);
	BIND_ENUM_CONSTANT(TYPE_SIMPLEX_FRACTAL);
#endif
	BIND_ENUM_CONSTANT(TYPE_WHITE_NOISE);
	BIND_ENUM_CONSTANT(TYPE_CELLULAR);
	BIND_ENUM_CONSTANT(TYPE_CUBIC);
	BIND_ENUM_CONSTANT(TYPE_CUBIC_FRACTAL);

	BIND_ENUM_CONSTANT(FRACTAL_FBM);
	BIND_ENUM_CONSTANT(FRACTAL_BILLOW);
	BIND_ENUM_CONSTANT(FRACTAL_RIDGED_MULTI);

	BIND_ENUM_CONSTANT(PERTURB_NONE);
	BIND_ENUM_CONSTANT(PERTURB_GRADIENT);
	BIND_ENUM_CONSTANT(PERTURB_GRADIENT_FRACTAL);
	BIND_ENUM_CONSTANT(PERTURB_NORMALIZE);
	BIND_ENUM_CONSTANT(PERTURB_GRADIENT_NORMALIZE);
	BIND_ENUM_CONSTANT(PERTURB_GRADIENT_FRACTAL_NORMALIZE);

	BIND_ENUM_CONSTANT(DISTANCE_EUCLIDEAN);
	BIND_ENUM_CONSTANT(DISTANCE_MANHATTAN);
	BIND_ENUM_CONSTANT(DISTANCE_NATURAL);

	BIND_ENUM_CONSTANT(RETURN_CELL_VALUE);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE2);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE2_ADD);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE2_SUB);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE2_MUL);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE2_DIV);
	BIND_ENUM_CONSTANT(RETURN_DISTANCE2_CAVE);
	BIND_ENUM_CONSTANT(RETURN_NOISE_LOOKUP);
}
