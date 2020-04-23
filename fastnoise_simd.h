/*************************************************************************/
/*  fast_noise_simd.h                                                    */
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

#ifndef FASTNOISE_SIMD_H
#define FASTNOISE_SIMD_H

#include "core/image.h"
#include "core/reference.h"
#include "modules/noise/noise.h"
#include "scene/resources/texture.h"

#include "thirdparty/FastNoiseSIMD.h"

class FastNoiseSIMD : public Noise {
	GDCLASS(FastNoiseSIMD, Noise);
	OBJ_SAVE_TYPE(FastNoiseSIMD);

public:
	enum NoiseType {
		TYPE_VALUE = _FastNoiseSIMD::Value,
		TYPE_VALUE_FRACTAL = _FastNoiseSIMD::ValueFractal,
		TYPE_PERLIN = _FastNoiseSIMD::Perlin,
		TYPE_PERLIN_FRACTAL = _FastNoiseSIMD::PerlinFractal,
#ifdef SIMPLEX_ENABLED
		TYPE_SIMPLEX = _FastNoiseSIMD::Simplex,
		TYPE_SIMPLEX_FRACTAL = _FastNoiseSIMD::SimplexFractal,
#endif
		TYPE_WHITE_NOISE = _FastNoiseSIMD::WhiteNoise,
		TYPE_CELLULAR = _FastNoiseSIMD::Cellular,
		TYPE_CUBIC = _FastNoiseSIMD::Cubic,
		TYPE_CUBIC_FRACTAL = _FastNoiseSIMD::CubicFractal
	};

	enum FractalType {
		FRACTAL_FBM = _FastNoiseSIMD::FBM,
		FRACTAL_BILLOW = _FastNoiseSIMD::Billow,
		FRACTAL_RIDGED_MULTI = _FastNoiseSIMD::RigidMulti
	};

	enum PerturbType {
		PERTURB_NONE = _FastNoiseSIMD::None,
		PERTURB_GRADIENT = _FastNoiseSIMD::Gradient,
		PERTURB_GRADIENT_FRACTAL = _FastNoiseSIMD::GradientFractal,
		PERTURB_NORMALIZE = _FastNoiseSIMD::Normalise,
		PERTURB_GRADIENT_NORMALIZE = _FastNoiseSIMD::Gradient_Normalise,
		PERTURB_GRADIENT_FRACTAL_NORMALIZE = _FastNoiseSIMD::GradientFractal_Normalise
	};

	enum CellularDistanceFunction {
		DISTANCE_EUCLIDEAN = _FastNoiseSIMD::Euclidean,
		DISTANCE_MANHATTAN = _FastNoiseSIMD::Manhattan,
		DISTANCE_NATURAL = _FastNoiseSIMD::Natural
	};

	enum CellularReturnType {
		RETURN_CELL_VALUE = _FastNoiseSIMD::CellValue,
		RETURN_DISTANCE = _FastNoiseSIMD::Distance,
		RETURN_DISTANCE2 = _FastNoiseSIMD::Distance2,
		RETURN_DISTANCE2_ADD = _FastNoiseSIMD::Distance2Add,
		RETURN_DISTANCE2_SUB = _FastNoiseSIMD::Distance2Sub,
		RETURN_DISTANCE2_MUL = _FastNoiseSIMD::Distance2Mul,
		RETURN_DISTANCE2_DIV = _FastNoiseSIMD::Distance2Div,
		RETURN_DISTANCE2_CAVE = _FastNoiseSIMD::Distance2Cave,
		RETURN_NOISE_LOOKUP = _FastNoiseSIMD::NoiseLookup

	};

	FastNoiseSIMD();
	~FastNoiseSIMD();

	// General noise settings

	void set_noise_type(NoiseType p_noise_type);
	NoiseType get_noise_type() const;

	void set_seed(int p_seed);
	int get_seed() const;

	void set_frequency(float p_freq);
	float get_frequency() const;

	void set_offset(Vector3 p_offset);
	Vector3 get_offset() const;

	void set_axis_scales(Vector3 p_scale);
	Vector3 get_axis_scales() const;

	void set_scale(float p_scale);
	float get_scale() const;

	int get_simd_level() const;

	// Noise singular value functions (These are very slow. 1/3rd the speed of FastNoise.)

	float get_noise_1d(float p_z);
	float get_noise_2dv(Vector2 p_v);
	float get_noise_2d(float p_x, float p_z);
	float get_noise_3dv(Vector3 p_v);
	float get_noise_3d(float p_x, float p_y, float p_z);

	// Noise set functions (There is a speed benefit if the Z channel is a multiple of 8.)

	float *get_noise_set_1d(float p_z, int p_sizez, float p_scale = 0.0f);
	float *get_noise_set_2dv(Vector2 p_v, Vector2 p_size, float p_scale = 0.0f);
	float *get_noise_set_2d(float p_x, float p_z, int p_sizex, int p_sizez, float p_scale = 0.0f);
	float *get_noise_set_3dv(Vector3 p_v, Vector3 p_size, float p_scale = 0.0f);
	float *get_noise_set_3d(int p_x, int p_y, int p_z, int p_sizex, int p_sizey, int p_sizez, float p_scale = 0.0f);
	float *get_empty_set_3dv(Vector3 p_size);
	float *get_empty_set(int p_size);
	void fill_noise_set_3dv(float *p_set, Vector3 p_v, Vector3 p_size, float p_scale = 0.0f);
	void fill_noise_set_3d(float *p_set, int p_x, int p_y, int p_z, int p_sizex, int p_sizey, int p_sizez, float p_scale = 0.0f);
	void free_noise_set(float *set);

	// Allocate PoolVectors for GDScript

	Vector<float> _b_get_noise_set_1d(float p_z, int p_sizez, float p_scale = 0.0f);
	Vector<float> _b_get_noise_set_2dv(Vector2 p_v, Vector2 p_size, float p_scale = 0.0f);
	Vector<float> _b_get_noise_set_3dv(Vector3 p_v, Vector3 p_size, float p_scale = 0.0f);

	// Perturb texture coordinates within the noise functions

	void set_perturb_type(PerturbType p_type);
	PerturbType get_perturb_type() const;

	void set_perturb_amplitude(float p_amp);
	float get_perturb_amplitude() const;

	void set_perturb_frequency(float p_freq);
	float get_perturb_frequency() const;

	void set_perturb_fractal_octaves(int p_octaves);
	int get_perturb_fractal_octaves();

	void set_perturb_fractal_lacunarity(float p_lacunarity);
	float get_perturb_fractal_lacunarity();

	void set_perturb_fractal_gain(float p_gain);
	float get_perturb_fractal_gain();

	void set_perturb_normalize_length(float p_length);
	float get_perturb_normalize_length();

	// Fractal specific

	void set_fractal_type(FractalType p_type);
	FractalType get_fractal_type() const;

	void set_fractal_octaves(int p_octaves);
	int get_fractal_octaves() const;

	void set_fractal_lacunarity(float p_lacunarity);
	float get_fractal_lacunarity() const;

	void set_fractal_gain(float p_gain);
	float get_fractal_gain() const;

	// Cellular specific

	void set_cellular_distance_function(CellularDistanceFunction p_func);
	CellularDistanceFunction get_cellular_distance_function() const;

	void set_cellular_return_type(CellularReturnType p_type);
	CellularReturnType get_cellular_return_type() const;

	void set_cellular_distance2_indices(int p_index0, int p_index1);
	PackedInt32Array get_cellular_distance2_indices() const;

	void set_cellular_distance2_index0(int p_index0); // Editor helpers
	int get_cellular_distance2_index0() const;
	void set_cellular_distance2_index1(int p_index1);
	int get_cellular_distance2_index1() const;

	void set_cellular_jitter(float p_jitter);
	float get_cellular_jitter() const;

	void set_cellular_noise_lookup_type(NoiseType p_type);
	NoiseType get_cellular_noise_lookup_type() const;

	void set_cellular_noise_lookup_frequency(float p_freq);
	float get_cellular_noise_lookup_frequency() const;

	// Generate Textures

	Ref<Image> get_image(int p_width, int p_height, bool p_invert = false);
	Ref<Image> get_seamless_image(int p_width, int p_height, bool p_invert = false);

protected:
	static void _bind_methods();
	virtual void _validate_property(PropertyInfo& property) const;

private:
	_FastNoiseSIMD *_noise;
	List<float *> _allocated_sets;
	Vector3 _offset;
	float _scale;

	// Store these locally as using directly from the library is awkward for various reasons
	float _perturb_amp;
	Vector3 _axis_scales;
	int _cell_dist_index0;
	int _cell_dist_index1;
};

VARIANT_ENUM_CAST(FastNoiseSIMD::NoiseType);
VARIANT_ENUM_CAST(FastNoiseSIMD::FractalType);
VARIANT_ENUM_CAST(FastNoiseSIMD::PerturbType);
VARIANT_ENUM_CAST(FastNoiseSIMD::CellularDistanceFunction);
VARIANT_ENUM_CAST(FastNoiseSIMD::CellularReturnType);

#endif // FASTNOISE_SIMD_H
