import ctypes
from ctypes import (
    c_void_p, c_char_p, c_int, c_double, c_ulong, c_long,
    POINTER, Structure, CFUNCTYPE
)

# --- Dependency Placeholders ---
# Since these are defined in external headers (dsp.h), we treat them as opaque pointers.
dsp_stream_p = c_void_p
complex_t = c_void_p  # Replace with a real struct if complex_t layout is known

# --- OpenVLBI Data Structures ---

class vlbi_node(Structure):
    _fields_ = [
        ("GeographicLocation", POINTER(c_double)),
        ("Location", POINTER(c_double)),
        ("Geo", c_int),
        ("Stream", dsp_stream_p),
        ("Name", c_char_p),
        ("Index", c_int)
    ]

class vlbi_baseline(Structure):
    _fields_ = [
        ("relative", c_int),
        ("locked", c_int),
        ("Target", POINTER(c_double)),
        ("Ra", c_double),
        ("Dec", c_double),
        ("baseline", POINTER(c_double)),
        ("u", c_double),
        ("v", c_double),
        ("delay", c_double),
        ("WaveLength", c_double),
        ("SampleRate", c_double),
        ("nodes_count", c_int),
        ("Nodes", POINTER(vlbi_node)),
        ("Name", c_char_p),
        ("Stream", dsp_stream_p)
    ]

class timespec_t(Structure):
    _fields_ = [
        ("tv_sec", ctypes.c_longlong),  # __time64_t or long depending on OS
        ("tv_nsec", ctypes.c_long)
    ]

# Opaque pointer representing the OpenVLBI context
vlbi_context = c_void_p

# Delegate Function Callback Type: double (*vlbi_func2_t)(double, double)
vlbi_func2_t = CFUNCTYPE(c_double, c_double, c_double)

# Dummy structure for align info placeholder
class dsp_align_info(Structure):
    _fields_ = [
        ("offset", c_double),
        ("scale_ratio", c_double)
    ]


class OpenVLBI:
    def __init__(self, lib_path="libopenvlbi"):
        """Initializes and loads the OpenVLBI shared library."""
        self.lib = ctypes.CDLL(lib_path)
        self._setup_signatures()

    def _setup_signatures(self):
        # Core Core Functions
        self.lib.vlbi_max_threads.argtypes = [c_ulong]
        self.lib.vlbi_max_threads.restype = c_ulong

        self.lib.vlbi_get_version.argtypes = []
        self.lib.vlbi_get_version.restype = c_char_p

        self.lib.vlbi_init.argtypes = []
        self.lib.vlbi_init.restype = vlbi_context

        self.lib.vlbi_exit.argtypes = [vlbi_context]
        self.lib.vlbi_exit.restype = None

        # Nodes API
        self.lib.vlbi_add_node.argtypes = [vlbi_context, dsp_stream_p, c_char_p, c_int]
        self.lib.vlbi_add_node.restype = None

        self.lib.vlbi_copy_node.argtypes = [vlbi_context, c_char_p, c_char_p]
        self.lib.vlbi_copy_node.restype = None

        self.lib.vlbi_get_node.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_get_node.restype = dsp_stream_p

        self.lib.vlbi_has_node.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_has_node.restype = c_int

        self.lib.vlbi_del_node.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_del_node.restype = None

        self.lib.vlbi_get_nodes.argtypes = [vlbi_context, POINTER(POINTER(vlbi_node))]
        self.lib.vlbi_get_nodes.restype = c_int

        self.lib.vlbi_add_node_from_fits.argtypes = [vlbi_context, c_char_p, c_char_p, c_int]
        self.lib.vlbi_add_node_from_fits.restype = None

        self.lib.vlbi_add_nodes_from_sdfits.argtypes = [vlbi_context, c_char_p, c_char_p, c_int]
        self.lib.vlbi_add_nodes_from_sdfits.restype = None

        # Filter API
        self.lib.vlbi_filter_lp_node.argtypes = [vlbi_context, c_char_p, c_char_p, c_double]
        self.lib.vlbi_filter_lp_node.restype = None
        self.lib.vlbi_filter_hp_node.argtypes = [vlbi_context, c_char_p, c_char_p, c_double]
        self.lib.vlbi_filter_hp_node.restype = None
        self.lib.vlbi_filter_bp_node.argtypes = [vlbi_context, c_char_p, c_char_p, c_double, c_double]
        self.lib.vlbi_filter_bp_node.restype = None
        self.lib.vlbi_filter_br_node.argtypes = [vlbi_context, c_char_p, c_char_p, c_double, c_double]
        self.lib.vlbi_filter_br_node.restype = None

        # Baselines API
        self.lib.vlbi_set_correlation_order.argtypes = [vlbi_context, c_int]
        self.lib.vlbi_set_correlation_order.restype = None

        self.lib.vlbi_get_baselines.argtypes = [vlbi_context, POINTER(POINTER(vlbi_baseline))]
        self.lib.vlbi_get_baselines.restype = c_int

        self.lib.vlbi_free_baselines.argtypes = [POINTER(POINTER(vlbi_baseline)), c_int]
        self.lib.vlbi_free_baselines.restype = None

        self.lib.vlbi_set_baseline_buffer.argtypes = [vlbi_context, POINTER(c_char_p), complex_t, c_int]
        self.lib.vlbi_set_baseline_buffer.restype = None

        self.lib.vlbi_get_baseline_stream.argtypes = [vlbi_context, POINTER(c_char_p)]
        self.lib.vlbi_get_baseline_stream.restype = dsp_stream_p

        self.lib.vlbi_unlock_baseline.argtypes = [vlbi_context, POINTER(c_char_p)]
        self.lib.vlbi_unlock_baseline.restype = None

        self.lib.vlbi_set_baseline_stream.argtypes = [vlbi_context, POINTER(c_char_p), dsp_stream_p]
        self.lib.vlbi_set_baseline_stream.restype = None

        self.lib.vlbi_set_location.argtypes = [vlbi_context, c_double, c_double, c_double]
        self.lib.vlbi_set_location.restype = None

        self.lib.vlbi_get_offset.argtypes = [vlbi_context, c_double, c_char_p, c_double, c_double, c_double]
        self.lib.vlbi_get_offset.restype = c_double

        # Models API
        self.lib.vlbi_get_uv_plot.argtypes = [
            vlbi_context, c_char_p, c_int, c_int, POINTER(c_double),
            c_double, c_double, c_int, c_int, vlbi_func2_t, POINTER(c_int)
        ]
        self.lib.vlbi_get_uv_plot.restype = None

        self.lib.vlbi_add_model.argtypes = [vlbi_context, dsp_stream_p, c_char_p]
        self.lib.vlbi_add_model.restype = None

        self.lib.vlbi_copy_model.argtypes = [vlbi_context, c_char_p, c_char_p]
        self.lib.vlbi_copy_model.restype = None

        self.lib.vlbi_del_model.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_del_model.restype = None

        self.lib.vlbi_get_models.argtypes = [vlbi_context, POINTER(POINTER(dsp_stream_p))]
        self.lib.vlbi_get_models.restype = c_int

        self.lib.vlbi_get_model.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_get_model.restype = dsp_stream_p

        self.lib.vlbi_has_model.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_has_model.restype = c_int

        self.lib.vlbi_get_ifft.argtypes = [vlbi_context, c_char_p, c_char_p, c_char_p]
        self.lib.vlbi_get_ifft.restype = None

        self.lib.vlbi_get_fft.argtypes = [vlbi_context, c_char_p, c_char_p, c_char_p]
        self.lib.vlbi_get_fft.restype = None

        self.lib.vlbi_apply_mask.argtypes = [vlbi_context, c_char_p, c_char_p, c_char_p]
        self.lib.vlbi_apply_mask.restype = None

        self.lib.vlbi_apply_convolution_matrix.argtypes = [vlbi_context, c_char_p, c_char_p, c_char_p]
        self.lib.vlbi_apply_convolution_matrix.restype = None

        self.lib.vlbi_stack_models.argtypes = [vlbi_context, c_char_p, c_char_p, c_char_p]
        self.lib.vlbi_stack_models.restype = None

        self.lib.vlbi_diff_models.argtypes = [vlbi_context, c_char_p, c_char_p, c_char_p]
        self.lib.vlbi_diff_models.restype = None

        self.lib.vlbi_compare_models.argtypes = [vlbi_context, c_char_p, c_char_p]
        self.lib.vlbi_compare_models.restype = c_double

        self.lib.vlbi_shift.argtypes = [vlbi_context, c_char_p]
        self.lib.vlbi_shift.restype = None

        # Image Formats Model Input/Output
        for method in ['png', 'jpeg', 'fits']:
            getattr(self.lib, f"vlbi_add_model_from_{method}").argtypes = [vlbi_context, c_char_p, c_char_p]
            getattr(self.lib, f"vlbi_add_model_from_{method}").restype = None
            getattr(self.lib, f"vlbi_get_model_to_{method}").argtypes = [vlbi_context, c_char_p, c_char_p]
            getattr(self.lib, f"vlbi_get_model_to_{method}").restype = None

        # Parallax Calculators (Matrix)
        self.lib.vlbi_matrix_calc_baseline_center.argtypes = [POINTER(c_double), POINTER(c_double)]
        self.lib.vlbi_matrix_calc_baseline_center.restype = POINTER(c_double)

        self.lib.vlbi_matrix_calc_3d_projection.argtypes = [c_double, c_double, POINTER(c_double)]
        self.lib.vlbi_matrix_calc_3d_projection.restype = POINTER(c_double)

        self.lib.vlbi_matrix_calc_parametric_projection.argtypes = [POINTER(c_double), POINTER(c_double)]
        self.lib.vlbi_matrix_calc_parametric_projection.restype = POINTER(c_double)

        self.lib.vlbi_matrix_calc_uv_coordinates.argtypes = [POINTER(c_double), c_double]
        self.lib.vlbi_matrix_calc_uv_coordinates.restype = POINTER(c_double)

        self.lib.vlbi_matrix_calc_location.argtypes = [POINTER(c_double)]
        self.lib.vlbi_matrix_calc_location.restype = POINTER(c_double)

        self.lib.vlbi_matrix_estimate_geocentric_elevation.argtypes = [c_double, c_double]
        self.lib.vlbi_matrix_estimate_geocentric_elevation.restype = c_double

        self.lib.vlbi_matrix_estimate_resolution_zero.argtypes = [c_double]
        self.lib.vlbi_matrix_estimate_resolution_zero.restype = c_double

        self.lib.vlbi_matrix_estimate_resolution.argtypes = [c_double, c_double]
        self.lib.vlbi_matrix_estimate_resolution.restype = c_double

        # Time Conversions
        self.lib.vlbi_time_mktimespec.argtypes = [c_int, c_int, c_int, c_int, c_int, c_int, c_long]
        self.lib.vlbi_time_mktimespec.restype = timespec_t

        self.lib.vlbi_time_timespec_to_J2000time.argtypes = [timespec_t]
        self.lib.vlbi_time_timespec_to_J2000time.restype = c_double

        self.lib.vlbi_time_J2000time_to_lst.argtypes = [c_double, c_double]
        self.lib.vlbi_time_J2000time_to_lst.restype = c_double

        self.lib.vlbi_time_string_to_timespec.argtypes = [c_char_p]
        self.lib.vlbi_time_string_to_timespec.restype = timespec_t

        self.lib.vlbi_time_J2000time_to_timespec.argtypes = [c_double]
        self.lib.vlbi_time_J2000time_to_timespec.restype = timespec_t

        # Astronomy Specific
        self.lib.vlbi_astro_mean_speed.argtypes = [c_double]
        self.lib.vlbi_astro_mean_speed.restype = c_double

        self.lib.vlbi_astro_alt_az_from_ra_dec.argtypes = [
            c_double, c_double, c_double, c_double, c_double, POINTER(c_double), POINTER(c_double)
        ]
        self.lib.vlbi_astro_alt_az_from_ra_dec.restype = None

        self.lib.vlbi_astro_get_local_hour_angle.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_get_local_hour_angle.restype = c_double

        self.lib.vlbi_astro_get_alt_az_coordinates.argtypes = [c_double, c_double, c_double, POINTER(c_double), POINTER(c_double)]
        self.lib.vlbi_astro_get_alt_az_coordinates.restype = None

        self.lib.vlbi_astro_load_spectrum.argtypes = [c_char_p]
        self.lib.vlbi_astro_load_spectrum.restype = dsp_stream_p

        self.lib.vlbi_astro_load_spectra_catalog.argtypes = [c_char_p, POINTER(POINTER(dsp_stream_p)), POINTER(c_int)]
        self.lib.vlbi_astro_load_spectra_catalog.restype = c_int

        self.lib.vlbi_astro_create_reference_catalog.argtypes = [POINTER(dsp_stream_p), c_int]
        self.lib.vlbi_astro_create_reference_catalog.restype = dsp_stream_p

        self.lib.vlbi_astro_save_spectrum.argtypes = [dsp_stream_p, c_char_p]
        self.lib.vlbi_astro_save_spectrum.restype = None

        self.lib.vlbi_astro_scan_spectrum.argtypes = [dsp_stream_p, c_int]
        self.lib.vlbi_astro_scan_spectrum.restype = None

        self.lib.vlbi_astro_align_spectra.argtypes = [dsp_stream_p, dsp_stream_p, c_int, c_double, c_double]
        self.lib.vlbi_astro_align_spectra.restype = dsp_align_info

        self.lib.vlbi_astro_diff_spectra.argtypes = [dsp_stream_p, dsp_stream_p, c_double]
        self.lib.vlbi_astro_diff_spectra.restype = c_double

        self.lib.vlbi_astro_flux_ratio.argtypes = [c_double, c_double, c_double]
        self.lib.vlbi_astro_flux_ratio.restype = c_double

        self.lib.vlbi_astro_estimate_brightness_temperature.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_brightness_temperature.restype = c_double

        self.lib.vlbi_astro_estimate_temperature.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_temperature.restype = c_double

        self.lib.vlbi_astro_estimate_flux.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_flux.restype = c_double

        self.lib.vlbi_astro_estimate_temperature_ratio.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_temperature_ratio.restype = c_double

        self.lib.vlbi_astro_estimate_size_ratio.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_size_ratio.restype = c_double

        self.lib.vlbi_astro_estimate_luminosity_ratio.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_luminosity_ratio.restype = c_double

        self.lib.vlbi_astro_estimate_distance_ratio.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_distance_ratio.restype = c_double

        self.lib.vlbi_astro_estimate_distance_parallax.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_distance_parallax.restype = c_double

        self.lib.vlbi_astro_estimate_redshift.argtypes = [c_double, c_int]
        self.lib.vlbi_astro_estimate_redshift.restype = c_double

        self.lib.vlbi_astro_estimate_size_transient.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_estimate_size_transient.restype = c_double

        self.lib.vlbi_astro_redshift_adjust.argtypes = [c_double, c_double]
        self.lib.vlbi_astro_redshift_adjust.restype = c_double

        self.lib.vlbi_file_read_fits.argtypes = [c_char_p]
        self.lib.vlbi_file_read_fits.restype = dsp_stream_p

        self.lib.vlbi_file_read_sdfits.argtypes = [c_char_p, POINTER(ctypes.c_long)]
        self.lib.vlbi_file_read_sdfits.restype = POINTER(dsp_stream_p)