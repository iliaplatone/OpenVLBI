import os
import ctypes
from ctypes import (
    Structure, Union, POINTER, c_int, c_double, c_ulong,
    c_char, c_void_p, c_char_p
)

# ==============================================================================
# 1. Load the shared library
# ==============================================================================
# Update 'libopendsp.so' to the full path or correct extension (.dll / .dylib) if needed
_lib_path = os.path.abspath("libopendsp")
try:
    _dsp = ctypes.CDLL(_lib_path)
except OSError as e:
    raise ImportError(f"Could not load libDSP shared library from {_lib_path}. Ensure it is compiled and accessible.") from e

# ==============================================================================
# 2. Map Defines & Basic Types
# ==============================================================================
DSP_MAX_STARS = 200
DSP_NAME_SIZE = 128

# Basic types mapped from C typedefs[cite: 1]
dsp_t = c_double             # typedef double dsp_t;[cite: 1]
complex_t = c_double * 2     # typedef double complex_t[2];[cite: 1]

# ==============================================================================
# 3. Struct & Union Implementations
# ==============================================================================

class multicomplex(Structure):
    _fields_ = [
        ("dims", c_int),
        ("real", c_double),
        ("imaginary", POINTER(c_double)),
    ]

class dsp_point(Structure):
    _fields_ = [
        ("location", POINTER(c_double)),
        ("dims", c_int),
    ]

class dsp_offset(Structure):
    _fields_ = [
        ("offset", POINTER(c_double)),
        ("dims", c_int),
    ]

class dsp_star(Structure):
    _fields_ = [
        ("center", dsp_point),
        ("diameter", c_double),
        ("peak", c_double),
        ("flux", c_double),
        ("theta", c_double),
        ("name", c_char * DSP_NAME_SIZE),
    ]

class dsp_triangle(Structure):
    pass
# dsp_triangle has self-referential connections down the line, so we define it properly here:
dsp_triangle._fields_ = [
    ("index", c_double),
    ("dims", c_int),
    ("theta", POINTER(c_double)),
    ("sizes", POINTER(c_double)),
    ("ratios", POINTER(c_double)),
    ("stars", POINTER(dsp_star)),
    ("stars_count", c_int),
]

class dsp_align_info(Structure):
    _fields_ = [
        ("offset", POINTER(c_double)),
        ("center", POINTER(c_double)),
        ("radians", POINTER(c_double)),
        ("factor", POINTER(c_double)),
        ("dims", c_int),
        ("triangles", dsp_triangle * 2),
        ("triangles_count", c_int),
        ("score", c_double),
        ("decimals", c_double),
        ("err", c_int),
    ]

class _ComplexStruct(Structure):
    _fields_ = [
        ("real", c_double),
        ("imaginary", c_double),
    ]

class dsp_complex(Union):
    _fields_ = [
        ("complex", POINTER(_ComplexStruct)),
        ("pairs", POINTER(complex_t)),
        ("buf", POINTER(c_double)),
    ]

class dsp_region(Structure):
    _fields_ = [
        ("start", c_int),
        ("len", c_int),
    ]

class _XYZ(Structure):
    _fields_ = [("x", c_double), ("y", c_double), ("z", c_double)]

class _Geographic(Structure):
    _fields_ = [("lon", c_double), ("lat", c_double), ("el", c_double)]

class dsp_location(Union):
    _fields_ = [
        ("xyz", _XYZ),
        ("geographic", _Geographic),
        ("coordinates", c_double * 3),
    ]

# Callback function prototype: void *(*dsp_func_t) (void *, ...);[cite: 1]
# Note: ctypes standard CFUNCTYPE doesn't natively support variadic C arguments seamlessly,
# but can handle fixed parameters. Defaulting generic callback below:
dsp_func_t = ctypes.CFUNCTYPE(c_void_p, c_void_p)

# timespec mapping for matching struct timespec starttimeutc;[cite: 1]
class timespec(Structure):
    _fields_ = [
        ("tv_sec", ctypes.c_longlong),
        ("tv_nsec", ctypes.c_longlong),
    ]

class dsp_stream(Structure):
    pass

dsp_stream_p = POINTER(dsp_stream)

# Recursive structure definition matching struct dsp_stream_t[cite: 1]
dsp_stream._fields_ = [
    ("name", c_char * DSP_NAME_SIZE),
    ("is_copy", c_int),
    ("len", c_int),
    ("dims", c_int),
    ("sizes", POINTER(c_int)),
    ("buf", POINTER(dsp_t)),
    ("dft", dsp_complex),
    ("arg", c_void_p),
    ("parent", dsp_stream_p),
    ("children", POINTER(dsp_stream_p)),
    ("child_count", c_int),
    ("location", POINTER(dsp_location)),
    ("target", POINTER(c_double)),
    ("starttimeutc", timespec),
    ("wavelength", c_double),
    ("focal_ratio", c_double),
    ("diameter", c_double),
    ("SNR", c_double),
    ("red", c_int),
    ("pixel_sizes", POINTER(c_double)),
    ("samplerate", c_double),
    ("thread", c_void_p), # Mapping pthread_t abstractly as a void pointer
    ("func", dsp_func_t),
    ("magnitude", dsp_stream_p),
    ("phase", dsp_stream_p),
    ("ROI", POINTER(dsp_region)),
    ("stars", POINTER(dsp_star)),
    ("stars_count", c_int),
    ("triangles", POINTER(dsp_triangle)),
    ("triangles_count", c_int),
    ("align_info", dsp_align_info),
    ("frame_number", c_int),
]

# ==============================================================================
# 4. Exposing C Function Prototypes
# ==============================================================================

# Threading and Debug Functions
_dsp.dsp_max_threads.argtypes = [c_ulong]
_dsp.dsp_max_threads.restype = c_ulong

_dsp.dsp_set_debug_level.argtypes = [c_int]
_dsp.dsp_set_debug_level.restype = None

_dsp.dsp_get_debug_level.argtypes = []
_dsp.dsp_get_debug_level.restype = c_int

_dsp.dsp_set_app_name.argtypes = [c_char_p]
_dsp.dsp_set_app_name.restype = None

_dsp.dsp_get_app_name.argtypes = []
_dsp.dsp_get_app_name.restype = c_char_p

_dsp.dsp_print.argtypes = [c_int, c_char_p]
_dsp.dsp_print.restype = None

# Fourier Functions
_dsp.dsp_fourier_dft.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_fourier_dft.restype = None

_dsp.dsp_fourier_idft.argtypes = [dsp_stream_p]
_dsp.dsp_fourier_idft.restype = None

_dsp.dsp_fourier_2dsp.argtypes = [dsp_stream_p]
_dsp.dsp_fourier_2dsp.restype = None

_dsp.dsp_fourier_2complex_t.argtypes = [dsp_stream_p]
_dsp.dsp_fourier_2complex_t.restype = None

_dsp.dsp_fourier_phase_mag_array_get_complex.argtypes = [POINTER(c_double), POINTER(c_double), POINTER(complex_t), c_int]
_dsp.dsp_fourier_phase_mag_array_get_complex.restype = None

_dsp.dsp_fourier_complex_array_get_magnitude.argtypes = [dsp_complex, c_int]
_dsp.dsp_fourier_complex_array_get_magnitude.restype = POINTER(c_double)

_dsp.dsp_fourier_complex_array_get_phase.argtypes = [dsp_complex, c_int]
_dsp.dsp_fourier_complex_array_get_phase.restype = POINTER(c_double)

# Filters
_dsp.dsp_filter_squarelaw.argtypes = [dsp_stream_p]
_dsp.dsp_filter_squarelaw.restype = None

_dsp.dsp_filter_lowpass.argtypes = [dsp_stream_p, c_double]
_dsp.dsp_filter_lowpass.restype = None

_dsp.dsp_filter_highpass.argtypes = [dsp_stream_p, c_double]
_dsp.dsp_filter_highpass.restype = None

_dsp.dsp_filter_bandpass.argtypes = [dsp_stream_p, c_double, c_double]
_dsp.dsp_filter_bandpass.restype = None

_dsp.dsp_filter_bandreject.argtypes = [dsp_stream_p, c_double, c_double]
_dsp.dsp_filter_bandreject.restype = None

# Convolution
_dsp.dsp_convolution_convolution.argtypes = [dsp_stream_p, dsp_stream_p]
_dsp.dsp_convolution_convolution.restype = None

_dsp.dsp_convolution_correlation.argtypes = [dsp_stream_p, dsp_stream_p]
_dsp.dsp_convolution_correlation.restype = None

# Stats
_dsp.dsp_stats_histogram.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stats_histogram.restype = POINTER(c_double)

# Buffer Operations
_dsp.dsp_buffer_shift.argtypes = [dsp_stream_p]
_dsp.dsp_buffer_shift.restype = None

_dsp.dsp_buffer_removemean.argtypes = [dsp_stream_p]
_dsp.dsp_buffer_removemean.restype = None

# Math operations mapping
for op_name in ['max', 'min', 'sub', 'sum', 'div', 'mul', 'pow', 'log']:
    func = getattr(_dsp, f"dsp_buffer_{op_name}")
    func.argtypes = [dsp_stream_p, POINTER(dsp_t), c_int]
    func.restype = None

for op_name in ['sub1', '1sub', 'sum1']:
    func = getattr(_dsp, f"dsp_buffer_{op_name}")
    func.argtypes = [dsp_stream_p, dsp_t]
    func.restype = None

for op_name in ['div1', '1div', 'mul1', 'pow1', 'log1']:
    func = getattr(_dsp, f"dsp_buffer_{op_name}")
    func.argtypes = [dsp_stream_p, c_double]
    func.restype = None

_dsp.dsp_buffer_median.argtypes = [dsp_stream_p, c_int, c_int]
_dsp.dsp_buffer_median.restype = None

_dsp.dsp_buffer_sigma.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_buffer_sigma.restype = None

_dsp.dsp_buffer_deviate.argtypes = [dsp_stream_p, POINTER(dsp_t), dsp_t, dsp_t]
_dsp.dsp_buffer_deviate.restype = None

# Stream Management Functions
_dsp.dsp_stream_alloc_buffer.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_alloc_buffer.restype = None

_dsp.dsp_stream_set_buffer.argtypes = [dsp_stream_p, c_void_p, c_int]
_dsp.dsp_stream_set_buffer.restype = None

_dsp.dsp_stream_get_buffer.argtypes = [dsp_stream_p]
_dsp.dsp_stream_get_buffer.restype = POINTER(dsp_t)

_dsp.dsp_stream_free_buffer.argtypes = [dsp_stream_p]
_dsp.dsp_stream_free_buffer.restype = None

_dsp.dsp_stream_new.argtypes = []
_dsp.dsp_stream_new.restype = dsp_stream_p

_dsp.dsp_stream_free.argtypes = [dsp_stream_p]
_dsp.dsp_stream_free.restype = None

_dsp.dsp_stream_copy.argtypes = [dsp_stream_p]
_dsp.dsp_stream_copy.restype = dsp_stream_p

_dsp.dsp_stream_add_child.argtypes = [dsp_stream_p, dsp_stream_p]
_dsp.dsp_stream_add_child.restype = None

_dsp.dsp_stream_add_star.argtypes = [dsp_stream_p, dsp_star]
_dsp.dsp_stream_add_star.restype = None

_dsp.dsp_stream_del_star.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_del_star.restype = None

_dsp.dsp_stream_add_triangle.argtypes = [dsp_stream_p, dsp_triangle]
_dsp.dsp_stream_add_triangle.restype = None

_dsp.dsp_stream_del_triangle.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_del_triangle.restype = None

_dsp.dsp_stream_calc_triangles.argtypes = [dsp_stream_p]
_dsp.dsp_stream_calc_triangles.restype = None

_dsp.dsp_stream_del_child.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_del_child.restype = None

_dsp.dsp_stream_add_dim.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_add_dim.restype = None

_dsp.dsp_stream_set_dim.argtypes = [dsp_stream_p, c_int, c_int]
_dsp.dsp_stream_set_dim.restype = None

_dsp.dsp_stream_del_dim.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_del_dim.restype = None

_dsp.dsp_stream_set_position.argtypes = [dsp_stream_p, POINTER(c_int)]
_dsp.dsp_stream_set_position.restype = c_int

_dsp.dsp_stream_get_position.argtypes = [dsp_stream_p, c_int]
_dsp.dsp_stream_get_position.restype = POINTER(c_int)

_dsp.dsp_stream_crop.argtypes = [dsp_stream_p]
_dsp.dsp_stream_crop.restype = None

_dsp.dsp_stream_rotate.argtypes = [dsp_stream_p]
_dsp.dsp_stream_rotate.restype = None

_dsp.dsp_stream_sum.argtypes = [dsp_stream_p, dsp_stream_p]
_dsp.dsp_stream_sum.restype = None

_dsp.dsp_stream_multiply.argtypes = [dsp_stream_p, dsp_stream_p]
_dsp.dsp_stream_multiply.restype = None

_dsp.dsp_stream_subtract.argtypes = [dsp_stream_p, dsp_stream_p]
_dsp.dsp_stream_subtract.restype = None

_dsp.dsp_stream_translate.argtypes = [dsp_stream_p]
_dsp.dsp_stream_translate.restype = None

_dsp.dsp_stream_scale.argtypes = [dsp_stream_p]
_dsp.dsp_stream_scale.restype = None

_dsp.dsp_stream_align.argtypes = [dsp_stream_p]
_dsp.dsp_stream_align.restype = None

# Signals Generator
_dsp.dsp_signals_whitenoise.argtypes = [dsp_stream_p]
_dsp.dsp_signals_whitenoise.restype = None

_dsp.dsp_signals_sinewave.argtypes = [dsp_stream_p, c_double, c_double]
_dsp.dsp_signals_sinewave.restype = None

_dsp.dsp_signals_sawtoothwave.argtypes = [dsp_stream_p, c_double, c_double]
_dsp.dsp_signals_sawtoothwave.restype = None

_dsp.dsp_signals_triwave.argtypes = [dsp_stream_p, c_double, c_double]
_dsp.dsp_signals_triwave.restype = None

_dsp.dsp_modulation_frequency.argtypes = [dsp_stream_p, c_double, c_double, c_double]
_dsp.dsp_modulation_frequency.restype = None

_dsp.dsp_modulation_amplitude.argtypes = [dsp_stream_p, c_double, c_double]
_dsp.dsp_modulation_amplitude.restype = None

# File IO Functions
_dsp.dsp_file_read_fits.argtypes = [c_char_p, POINTER(c_int), c_int]
_dsp.dsp_file_read_fits.restype = POINTER(dsp_stream_p)

_dsp.dsp_file_write_fits.argtypes = [c_char_p, c_int, dsp_stream_p]
_dsp.dsp_file_write_fits.restype = None

_dsp.dsp_file_read_jpeg.argtypes = [c_char_p, POINTER(c_int), c_int]
_dsp.dsp_file_read_jpeg.restype = POINTER(dsp_stream_p)

_dsp.dsp_file_write_jpeg.argtypes = [c_char_p, c_int, dsp_stream_p]
_dsp.dsp_file_write_jpeg.restype = None

_dsp.dsp_file_read_png.argtypes = [c_char_p, POINTER(c_int), c_int]
_dsp.dsp_file_read_png.restype = POINTER(dsp_stream_p)

# Image Formats Converter APIs
_dsp.dsp_file_bayer_2_gray.argtypes = [POINTER(dsp_t), c_int, c_int]
_dsp.dsp_file_bayer_2_gray.restype = POINTER(dsp_t)

_dsp.dsp_file_bayer_2_rgb.argtypes = [POINTER(dsp_t), c_int, c_int, c_int]
_dsp.dsp_file_bayer_2_rgb.restype = POINTER(dsp_t)

_dsp.dsp_stream_from_components.argtypes = [POINTER(dsp_t), c_int, POINTER(c_int), c_int]
_dsp.dsp_stream_from_components.restype = POINTER(dsp_stream_p)

# Alignment APIs
_dsp.dsp_align_fill_info.argtypes = [dsp_triangle, dsp_triangle]
_dsp.dsp_align_fill_info.restype = POINTER(dsp_align_info)

_dsp.dsp_align_calc_triangle.argtypes = [POINTER(dsp_star), c_int]
_dsp.dsp_align_calc_triangle.restype = POINTER(dsp_triangle)

_dsp.dsp_align_free_triangle.argtypes = [POINTER(dsp_triangle)]
_dsp.dsp_align_free_triangle.restype = None

_dsp.dsp_align_get_offset.argtypes = [dsp_stream_p, dsp_stream_p, c_double, c_double, c_int]
_dsp.dsp_align_get_offset.restype = c_int

# Explicitly export the ctypes reference handler under standard naming
libdsp = _dsp