/*
 * Synaptics DSX touchscreen driver
 *
 * Copyright (C) 2012-2016 Synaptics Incorporated. All rights reserved.
 *
 * Copyright (C) 2012 Alexandra Chin <alexandra.chin@tw.synaptics.com>
 * Copyright (C) 2012 Scott Lin <scott.lin@tw.synaptics.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * INFORMATION CONTAINED IN THIS DOCUMENT IS PROVIDED "AS-IS," AND SYNAPTICS
 * EXPRESSLY DISCLAIMS ALL EXPRESS AND IMPLIED WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * AND ANY WARRANTIES OF NON-INFRINGEMENT OF ANY INTELLECTUAL PROPERTY RIGHTS.
 * IN NO EVENT SHALL SYNAPTICS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, PUNITIVE, OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OF THE INFORMATION CONTAINED IN THIS DOCUMENT, HOWEVER CAUSED
 * AND BASED ON ANY THEORY OF LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, AND EVEN IF SYNAPTICS WAS ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE. IF A TRIBUNAL OF COMPETENT JURISDICTION DOES
 * NOT PERMIT THE DISCLAIMER OF DIRECT DAMAGES OR ANY OTHER DAMAGES, SYNAPTICS'
 * TOTAL CUMULATIVE LIABILITY TO ANY PARTY SHALL NOT EXCEED ONE HUNDRED U.S.
 * DOLLARS.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/input/synaptics_dsx_htc.h>
#include "synaptics_dsx_core.h"
#ifdef KERNEL_ABOVE_2_6_38
#include <linux/input/mt.h>
#endif

#if defined(CONFIG_SECURE_TOUCH)
#include <linux/errno.h>
#include <soc/qcom/scm.h>
enum subsystem {
                TZ = 1,
                APSS = 3
};

#define TZ_BLSP_MODIFY_OWNERSHIP_ID 3
#endif

#ifdef HTC_FEATURE
#include <linux/async.h>
#endif

#define INPUT_PHYS_NAME "synaptics_dsx/touch_input"
#define STYLUS_PHYS_NAME "synaptics_dsx/stylus"

#define VIRTUAL_KEY_MAP_FILE_NAME "virtualkeys." PLATFORM_DRIVER_NAME

#ifdef KERNEL_ABOVE_2_6_38
#define TYPE_B_PROTOCOL
#endif

/*
#define USE_DATA_SERVER
*/

#define WAKEUP_GESTURE false

#define NO_0D_WHILE_2D
#define REPORT_2D_Z
#define REPORT_2D_W
/*
#define REPORT_2D_PRESSURE
*/

#define F12_DATA_15_WORKAROUND

#define IGNORE_FN_INIT_FAILURE
/*
#define FB_READY_RESET
#define FB_READY_WAIT_MS 100
#define FB_READY_TIMEOUT_S 30
*/
#ifdef SYNA_TDDI
#define TDDI_LPWG_WAIT_US 10
#endif
#define RPT_TYPE (1 << 0)
#define RPT_X_LSB (1 << 1)
#define RPT_X_MSB (1 << 2)
#define RPT_Y_LSB (1 << 3)
#define RPT_Y_MSB (1 << 4)
#define RPT_Z (1 << 5)
#define RPT_WX (1 << 6)
#define RPT_WY (1 << 7)
#define RPT_DEFAULT (RPT_TYPE | RPT_X_LSB | RPT_X_MSB | RPT_Y_LSB | RPT_Y_MSB)

#define REBUILD_WORK_DELAY_MS 500 /* ms */

#define EXP_FN_WORK_DELAY_MS 500 /* ms */
#define MAX_F11_TOUCH_WIDTH 15
#define MAX_F12_TOUCH_WIDTH 255

#define CHECK_STATUS_TIMEOUT_MS 100

#define F01_STD_QUERY_LEN 21
#define F01_BUID_ID_OFFSET 18
#ifdef HTC_FEATURE
#define F01_CHIP_ID_OFFSET 17
#endif

#define STATUS_NO_ERROR 0x00
#define STATUS_RESET_OCCURRED 0x01
#define STATUS_INVALID_CONFIG 0x02
#define STATUS_DEVICE_FAILURE 0x03
#define STATUS_CONFIG_CRC_FAILURE 0x04
#define STATUS_FIRMWARE_CRC_FAILURE 0x05
#define STATUS_CRC_IN_PROGRESS 0x06
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
#define STATUS_ESD_DETECTED 0x09
#endif

#define NORMAL_OPERATION (0 << 0)
#define SENSOR_SLEEP (1 << 0)
#define NO_SLEEP_OFF (0 << 2)
#define NO_SLEEP_ON (1 << 2)
#define CONFIGURED (1 << 7)

#define F11_CONTINUOUS_MODE 0x00
#define F11_WAKEUP_GESTURE_MODE 0x04
#define F12_CONTINUOUS_MODE 0x00
#define F12_WAKEUP_GESTURE_MODE 0x02
#define F12_UDG_DETECT 0x0f

#ifdef HTC_FEATURE
#define REPORT_INDEX_OFFSET 1
#define REPORT_DATA_OFFSET 3
#define F54_16BIT_IMAGE 2
#define F54_RAW_16BIT_IMAGE 3
#define F54_ABS_HYBRID_DELTA_CAP 59
#define F54_ABS_HYBRID_RAW_CAP 63

//#define GLOVE_MODE_BIT BIT(0)
//#define COVER_MODE_BIT BIT(1)
#define F54_CMD_FORCE_UPDATE 0x4
#define F54_CMD_FORCE_UPDATE_SIZE 1

#define F54_CONTROL_0_SIZE 1
#define F54_CONTROL_1_SIZE 1
#define F54_CONTROL_2_SIZE 2
#define F54_CONTROL_3_SIZE 1
#define F54_CONTROL_4_6_SIZE 3
#define F54_CONTROL_7_SIZE 1
#define F54_CONTROL_8_9_SIZE 3
#define F54_CONTROL_10_SIZE 1
#define F54_CONTROL_11_SIZE 2
#define F54_CONTROL_12_13_SIZE 2
#define F54_CONTROL_14_SIZE 1
#define F54_CONTROL_15_SIZE 1
#define F54_CONTROL_16_SIZE 1
#define F54_CONTROL_17_SIZE 1
#define F54_CONTROL_18_SIZE 1
#define F54_CONTROL_19_SIZE 1
#define F54_CONTROL_20_SIZE 1
#define F54_CONTROL_21_SIZE 2
#define F54_CONTROL_22_26_SIZE 7
#define F54_CONTROL_27_SIZE 1
#define F54_CONTROL_28_SIZE 2
#define F54_CONTROL_29_SIZE 1
#define F54_CONTROL_30_SIZE 1
#define F54_CONTROL_31_SIZE 1
#define F54_CONTROL_32_35_SIZE 8
#define F54_CONTROL_36_SIZE 1
#define F54_CONTROL_37_SIZE 1
#define F54_CONTROL_38_SIZE 1
#define F54_CONTROL_39_SIZE 1
#define F54_CONTROL_40_SIZE 1
#define F54_CONTROL_41_SIZE 1
#define F54_CONTROL_42_SIZE 2
#define F54_CONTROL_43_54_SIZE 13
#define F54_CONTROL_55_56_SIZE 2
#define F54_CONTROL_57_SIZE 1
#define F54_CONTROL_58_SIZE 1
#define F54_CONTROL_59_SIZE 2
#define F54_CONTROL_60_62_SIZE 3
#define F54_CONTROL_63_SIZE 1
#define F54_CONTROL_64_67_SIZE 4
#define F54_CONTROL_68_73_SIZE 8
#define F54_CONTROL_70_73_SIZE 6
#define F54_CONTROL_74_SIZE 2
#define F54_CONTROL_75_SIZE 1
#define F54_CONTROL_76_SIZE 1
#define F54_CONTROL_77_78_SIZE 2
#define F54_CONTROL_79_83_SIZE 5
#define F54_CONTROL_84_85_SIZE 2
#define F54_CONTROL_86_SIZE 1
#define F54_CONTROL_87_SIZE 1
#define F54_CONTROL_88_SIZE 1
#define F54_CONTROL_89_SIZE 1
#define F54_CONTROL_90_SIZE 1
#define F54_CONTROL_91_SIZE 1
#define F54_CONTROL_92_SIZE 1
#define F54_CONTROL_93_SIZE 1
#define F54_CONTROL_94_SIZE 1
#define F54_CONTROL_95_SIZE 1
#define F54_CONTROL_96_SIZE 1
#define F54_CONTROL_97_SIZE 1
#define F54_CONTROL_98_SIZE 1
#define F54_CONTROL_99_SIZE 1
#define F54_CONTROL_100_SIZE 1
#define F54_CONTROL_101_SIZE 1
#define F54_CONTROL_102_SIZE 1
#define F54_CONTROL_103_SIZE 1
#define F54_CONTROL_104_SIZE 1
#define F54_CONTROL_105_SIZE 1
#define F54_CONTROL_106_SIZE 1
#define F54_CONTROL_107_SIZE 1
#define F54_CONTROL_108_SIZE 1
#define F54_CONTROL_109_SIZE 1
#define F54_CONTROL_110_SIZE 1
#define F54_CONTROL_111_SIZE 1
#define F54_CONTROL_112_SIZE 1
#define F54_CONTROL_113_SIZE 1

#define MAX_SET_MODE_REG_SIZE 6

#define V5V6_CONFIG_ID_SIZE 4
#define V7_CONFIG_ID_SIZE 32

#define RESOL_SHIFT_BITS (10)
#define FREQ_MASK 0x7F

extern char *htc_get_bootmode(void);

static DECLARE_WAIT_QUEUE_HEAD(syn_data_ready_wq);

static unsigned int tamper_flag = 0;

static int synaptics_rmi4_hw_reset(struct synaptics_rmi4_data *rmi4_data);

#ifdef CONFIG_TOUCHSCREEN_USE_DSI_NOTIFY
static void dsi_status_detect(int status);
#endif
#endif
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
void (*set_lcm_rst)(void);

static void synaptics_rmi4_set_configured(struct synaptics_rmi4_data *rmi4_data);
#endif

static int synaptics_rmi4_check_status(struct synaptics_rmi4_data *rmi4_data,
		bool *was_in_bl_mode);
static int synaptics_rmi4_free_fingers(struct synaptics_rmi4_data *rmi4_data);
static int synaptics_rmi4_reinit_device(struct synaptics_rmi4_data *rmi4_data);
static int synaptics_rmi4_reset_device(struct synaptics_rmi4_data *rmi4_data,
		bool rebuild);

#ifdef CONFIG_FB
static int synaptics_rmi4_fb_notifier_cb(struct notifier_block *self,
		unsigned long event, void *data);
#endif
#ifdef CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY
static int synaptics_rmi4_drm_notifier_cb(struct notifier_block *self,
		unsigned long event, void *data);
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
#ifndef CONFIG_FB
#if defined(HTC_FEATURE) && !defined(CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY)
#define USE_EARLYSUSPEND
#endif
#endif
#endif

#ifdef USE_EARLYSUSPEND
static void synaptics_rmi4_early_suspend(struct early_suspend *h);

static void synaptics_rmi4_late_resume(struct early_suspend *h);
#endif

static int synaptics_rmi4_suspend(struct device *dev);

static int synaptics_rmi4_resume(struct device *dev);

static ssize_t synaptics_rmi4_f01_reset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count);

static ssize_t synaptics_rmi4_f01_productinfo_show(struct device *dev,
		struct device_attribute *attr, char *buf);

static ssize_t synaptics_rmi4_f01_buildid_show(struct device *dev,
		struct device_attribute *attr, char *buf);

static ssize_t synaptics_rmi4_f01_flashprog_show(struct device *dev,
		struct device_attribute *attr, char *buf);

static ssize_t synaptics_rmi4_0dbutton_show(struct device *dev,
		struct device_attribute *attr, char *buf);

static ssize_t synaptics_rmi4_0dbutton_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count);

static ssize_t synaptics_rmi4_suspend_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count);

static ssize_t synaptics_rmi4_wake_gesture_show(struct device *dev,
		struct device_attribute *attr, char *buf);

static ssize_t synaptics_rmi4_wake_gesture_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count);

#ifdef USE_DATA_SERVER
static ssize_t synaptics_rmi4_synad_pid_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count);
#endif

static ssize_t synaptics_rmi4_virtual_key_map_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf);

#ifdef HTC_FEATURE
enum switch_mode {
	SWITCH_TO_CPU = 0,
	SWITCH_TO_MCU,
	SWITCH_TO_MCU_AOD,
};

enum finger_dnup {
	REPORT_FINGER_UP = 0,
	REPORT_FINGER_DOWN,
	REPORT_FINGER_MOVE,
};

enum f34_version {
	F34_V0 = 0,
	F34_V1,
	F34_V2,
};

enum bl_version {
	BL_V5 = 5,
	BL_V6 = 6,
	BL_V7 = 7,
	BL_V8 = 8,
};
#endif

#if defined(CONFIG_SECURE_TOUCH)
static ssize_t synaptics_secure_touch_enable_show(struct device *dev,
                                struct device_attribute *attr, char *buf);

static ssize_t synaptics_secure_touch_enable_store(struct device *dev,
                                struct device_attribute *attr, const char *buf, size_t count);

static ssize_t synaptics_secure_touch_show(struct device *dev,
                                struct device_attribute *attr, char *buf);
#endif

struct synaptics_rmi4_f01_device_status {
	union {
		struct {
			unsigned char status_code:4;
			unsigned char reserved:2;
			unsigned char flash_prog:1;
			unsigned char unconfigured:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f11_query_0_5 {
	union {
		struct {
			/* query 0 */
			unsigned char f11_query0_b0__2:3;
			unsigned char has_query_9:1;
			unsigned char has_query_11:1;
			unsigned char has_query_12:1;
			unsigned char has_query_27:1;
			unsigned char has_query_28:1;

			/* query 1 */
			unsigned char num_of_fingers:3;
			unsigned char has_rel:1;
			unsigned char has_abs:1;
			unsigned char has_gestures:1;
			unsigned char has_sensitibity_adjust:1;
			unsigned char f11_query1_b7:1;

			/* query 2 */
			unsigned char num_of_x_electrodes;

			/* query 3 */
			unsigned char num_of_y_electrodes;

			/* query 4 */
			unsigned char max_electrodes:7;
			unsigned char f11_query4_b7:1;

			/* query 5 */
			unsigned char abs_data_size:2;
			unsigned char has_anchored_finger:1;
			unsigned char has_adj_hyst:1;
			unsigned char has_dribble:1;
			unsigned char has_bending_correction:1;
			unsigned char has_large_object_suppression:1;
			unsigned char has_jitter_filter:1;
		} __packed;
		unsigned char data[6];
	};
};

struct synaptics_rmi4_f11_query_7_8 {
	union {
		struct {
			/* query 7 */
			unsigned char has_single_tap:1;
			unsigned char has_tap_and_hold:1;
			unsigned char has_double_tap:1;
			unsigned char has_early_tap:1;
			unsigned char has_flick:1;
			unsigned char has_press:1;
			unsigned char has_pinch:1;
			unsigned char has_chiral_scroll:1;

			/* query 8 */
			unsigned char has_palm_detect:1;
			unsigned char has_rotate:1;
			unsigned char has_touch_shapes:1;
			unsigned char has_scroll_zones:1;
			unsigned char individual_scroll_zones:1;
			unsigned char has_multi_finger_scroll:1;
			unsigned char has_multi_finger_scroll_edge_motion:1;
			unsigned char has_multi_finger_scroll_inertia:1;
		} __packed;
		unsigned char data[2];
	};
};

struct synaptics_rmi4_f11_query_9 {
	union {
		struct {
			unsigned char has_pen:1;
			unsigned char has_proximity:1;
			unsigned char has_large_object_sensitivity:1;
			unsigned char has_suppress_on_large_object_detect:1;
			unsigned char has_two_pen_thresholds:1;
			unsigned char has_contact_geometry:1;
			unsigned char has_pen_hover_discrimination:1;
			unsigned char has_pen_hover_and_edge_filters:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f11_query_12 {
	union {
		struct {
			unsigned char has_small_object_detection:1;
			unsigned char has_small_object_detection_tuning:1;
			unsigned char has_8bit_w:1;
			unsigned char has_2d_adjustable_mapping:1;
			unsigned char has_general_information_2:1;
			unsigned char has_physical_properties:1;
			unsigned char has_finger_limit:1;
			unsigned char has_linear_cofficient_2:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f11_query_27 {
	union {
		struct {
			unsigned char f11_query27_b0:1;
			unsigned char has_pen_position_correction:1;
			unsigned char has_pen_jitter_filter_coefficient:1;
			unsigned char has_group_decomposition:1;
			unsigned char has_wakeup_gesture:1;
			unsigned char has_small_finger_correction:1;
			unsigned char has_data_37:1;
			unsigned char f11_query27_b7:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f11_ctrl_6_9 {
	union {
		struct {
			unsigned char sensor_max_x_pos_7_0;
			unsigned char sensor_max_x_pos_11_8:4;
			unsigned char f11_ctrl7_b4__7:4;
			unsigned char sensor_max_y_pos_7_0;
			unsigned char sensor_max_y_pos_11_8:4;
			unsigned char f11_ctrl9_b4__7:4;
		} __packed;
		unsigned char data[4];
	};
};

struct synaptics_rmi4_f11_data_1_5 {
	union {
		struct {
			unsigned char x_position_11_4;
			unsigned char y_position_11_4;
			unsigned char x_position_3_0:4;
			unsigned char y_position_3_0:4;
			unsigned char wx:4;
			unsigned char wy:4;
			unsigned char z;
		} __packed;
		unsigned char data[5];
	};
};

struct synaptics_rmi4_f12_query_5 {
	union {
		struct {
			unsigned char size_of_query6;
			struct {
				unsigned char ctrl0_is_present:1;
				unsigned char ctrl1_is_present:1;
				unsigned char ctrl2_is_present:1;
				unsigned char ctrl3_is_present:1;
				unsigned char ctrl4_is_present:1;
				unsigned char ctrl5_is_present:1;
				unsigned char ctrl6_is_present:1;
				unsigned char ctrl7_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl8_is_present:1;
				unsigned char ctrl9_is_present:1;
				unsigned char ctrl10_is_present:1;
				unsigned char ctrl11_is_present:1;
				unsigned char ctrl12_is_present:1;
				unsigned char ctrl13_is_present:1;
				unsigned char ctrl14_is_present:1;
				unsigned char ctrl15_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl16_is_present:1;
				unsigned char ctrl17_is_present:1;
				unsigned char ctrl18_is_present:1;
				unsigned char ctrl19_is_present:1;
				unsigned char ctrl20_is_present:1;
				unsigned char ctrl21_is_present:1;
				unsigned char ctrl22_is_present:1;
				unsigned char ctrl23_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl24_is_present:1;
				unsigned char ctrl25_is_present:1;
				unsigned char ctrl26_is_present:1;
				unsigned char ctrl27_is_present:1;
				unsigned char ctrl28_is_present:1;
				unsigned char ctrl29_is_present:1;
				unsigned char ctrl30_is_present:1;
				unsigned char ctrl31_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl32_is_present:1;
				unsigned char ctrl33_is_present:1;
				unsigned char ctrl34_is_present:1;
				unsigned char ctrl35_is_present:1;
				unsigned char ctrl36_is_present:1;
				unsigned char ctrl37_is_present:1;
				unsigned char ctrl38_is_present:1;
				unsigned char ctrl39_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl40_is_present:1;
				unsigned char ctrl41_is_present:1;
				unsigned char ctrl42_is_present:1;
				unsigned char ctrl43_is_present:1;
				unsigned char ctrl44_is_present:1;
				unsigned char ctrl45_is_present:1;
				unsigned char ctrl46_is_present:1;
				unsigned char ctrl47_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl48_is_present:1;
				unsigned char ctrl49_is_present:1;
				unsigned char ctrl50_is_present:1;
				unsigned char ctrl51_is_present:1;
				unsigned char ctrl52_is_present:1;
				unsigned char ctrl53_is_present:1;
				unsigned char ctrl54_is_present:1;
				unsigned char ctrl55_is_present:1;
			} __packed;
			struct {
				unsigned char ctrl56_is_present:1;
				unsigned char ctrl57_is_present:1;
				unsigned char ctrl58_is_present:1;
				unsigned char ctrl59_is_present:1;
				unsigned char ctrl60_is_present:1;
				unsigned char ctrl61_is_present:1;
				unsigned char ctrl62_is_present:1;
				unsigned char ctrl63_is_present:1;
			} __packed;
		};
		unsigned char data[9];
	};
};

struct synaptics_rmi4_f12_query_8 {
	union {
		struct {
			unsigned char size_of_query9;
			struct {
				unsigned char data0_is_present:1;
				unsigned char data1_is_present:1;
				unsigned char data2_is_present:1;
				unsigned char data3_is_present:1;
				unsigned char data4_is_present:1;
				unsigned char data5_is_present:1;
				unsigned char data6_is_present:1;
				unsigned char data7_is_present:1;
			} __packed;
			struct {
				unsigned char data8_is_present:1;
				unsigned char data9_is_present:1;
				unsigned char data10_is_present:1;
				unsigned char data11_is_present:1;
				unsigned char data12_is_present:1;
				unsigned char data13_is_present:1;
				unsigned char data14_is_present:1;
				unsigned char data15_is_present:1;
			} __packed;
			struct {
				unsigned char data16_is_present:1;
				unsigned char data17_is_present:1;
				unsigned char data18_is_present:1;
				unsigned char data19_is_present:1;
				unsigned char data20_is_present:1;
				unsigned char data21_is_present:1;
				unsigned char data22_is_present:1;
				unsigned char data23_is_present:1;
			} __packed;
			struct {
				unsigned char data24_is_present:1;
				unsigned char data25_is_present:1;
				unsigned char data26_is_present:1;
				unsigned char data27_is_present:1;
				unsigned char data28_is_present:1;
				unsigned char data29_is_present:1;
				unsigned char data30_is_present:1;
				unsigned char data31_is_present:1;
			} __packed;
		};
		unsigned char data[5];
	};
};

struct synaptics_rmi4_f12_ctrl_8 {
	union {
		struct {
			unsigned char max_x_coord_lsb;
			unsigned char max_x_coord_msb;
			unsigned char max_y_coord_lsb;
			unsigned char max_y_coord_msb;
			unsigned char rx_pitch_lsb;
			unsigned char rx_pitch_msb;
			unsigned char tx_pitch_lsb;
			unsigned char tx_pitch_msb;
			unsigned char low_rx_clip;
			unsigned char high_rx_clip;
			unsigned char low_tx_clip;
			unsigned char high_tx_clip;
			unsigned char num_of_rx;
			unsigned char num_of_tx;
		};
		unsigned char data[14];
	};
};

struct synaptics_rmi4_f12_ctrl_23 {
	union {
		struct {
			unsigned char finger_enable:1;
			unsigned char active_stylus_enable:1;
			unsigned char palm_enable:1;
			unsigned char unclassified_object_enable:1;
			unsigned char hovering_finger_enable:1;
			unsigned char gloved_finger_enable:1;
			unsigned char f12_ctr23_00_b6__7:2;
			unsigned char max_reported_objects;
			unsigned char f12_ctr23_02_b0:1;
			unsigned char report_active_stylus_as_finger:1;
			unsigned char report_palm_as_finger:1;
			unsigned char report_unclassified_object_as_finger:1;
			unsigned char report_hovering_finger_as_finger:1;
			unsigned char report_gloved_finger_as_finger:1;
			unsigned char report_narrow_object_swipe_as_finger:1;
			unsigned char report_handedge_as_finger:1;
			unsigned char cover_enable:1;
			unsigned char stylus_enable:1;
			unsigned char eraser_enable:1;
			unsigned char small_object_enable:1;
			unsigned char f12_ctr23_03_b4__7:4;
			unsigned char report_cover_as_finger:1;
			unsigned char report_stylus_as_finger:1;
			unsigned char report_eraser_as_finger:1;
			unsigned char report_small_object_as_finger:1;
			unsigned char f12_ctr23_04_b4__7:4;
		};
		unsigned char data[5];
	};
};

struct synaptics_rmi4_f12_ctrl_31 {
	union {
		struct {
			unsigned char max_x_coord_lsb;
			unsigned char max_x_coord_msb;
			unsigned char max_y_coord_lsb;
			unsigned char max_y_coord_msb;
			unsigned char rx_pitch_lsb;
			unsigned char rx_pitch_msb;
			unsigned char rx_clip_low;
			unsigned char rx_clip_high;
			unsigned char wedge_clip_low;
			unsigned char wedge_clip_high;
			unsigned char num_of_p;
			unsigned char num_of_q;
		};
		unsigned char data[12];
	};
};

struct synaptics_rmi4_f12_ctrl_58 {
	union {
		struct {
			unsigned char reporting_format;
			unsigned char f12_ctr58_00_reserved;
			unsigned char min_force_lsb;
			unsigned char min_force_msb;
			unsigned char max_force_lsb;
			unsigned char max_force_msb;
			unsigned char light_press_threshold_lsb;
			unsigned char light_press_threshold_msb;
			unsigned char light_press_hysteresis_lsb;
			unsigned char light_press_hysteresis_msb;
			unsigned char hard_press_threshold_lsb;
			unsigned char hard_press_threshold_msb;
			unsigned char hard_press_hysteresis_lsb;
			unsigned char hard_press_hysteresis_msb;
		};
		unsigned char data[14];
	};
};

struct synaptics_rmi4_f12_finger_data {
	unsigned char object_type_and_status;
	unsigned char x_lsb;
	unsigned char x_msb;
	unsigned char y_lsb;
	unsigned char y_msb;
#ifdef REPORT_2D_Z
	unsigned char z;
#endif
#ifdef REPORT_2D_W
	unsigned char wx;
	unsigned char wy;
#endif
};

struct synaptics_rmi4_f1a_query {
	union {
		struct {
			unsigned char max_button_count:3;
			unsigned char f1a_query0_b3__4:2;
			unsigned char has_query4:1;
			unsigned char has_query3:1;
			unsigned char has_query2:1;
			unsigned char has_general_control:1;
			unsigned char has_interrupt_enable:1;
			unsigned char has_multibutton_select:1;
			unsigned char has_tx_rx_map:1;
			unsigned char has_perbutton_threshold:1;
			unsigned char has_release_threshold:1;
			unsigned char has_strongestbtn_hysteresis:1;
			unsigned char has_filter_strength:1;
		} __packed;
		unsigned char data[2];
	};
};

struct synaptics_rmi4_f1a_query_4 {
	union {
		struct {
			unsigned char has_ctrl19:1;
			unsigned char f1a_query4_b1__4:4;
			unsigned char has_ctrl24:1;
			unsigned char f1a_query4_b6__7:2;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f1a_control_0 {
	union {
		struct {
			unsigned char multibutton_report:2;
			unsigned char filter_mode:2;
			unsigned char reserved:4;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f1a_control {
	struct synaptics_rmi4_f1a_control_0 general_control;
	unsigned char button_int_enable;
	unsigned char multi_button;
	unsigned char *txrx_map;
	unsigned char *button_threshold;
	unsigned char button_release_threshold;
	unsigned char strongest_button_hysteresis;
	unsigned char filter_strength;
};

struct synaptics_rmi4_f1a_handle {
	int button_bitmask_size;
	unsigned char max_count;
	unsigned char valid_button_count;
	unsigned char *button_data_buffer;
	unsigned char *button_map;
	struct synaptics_rmi4_f1a_query button_query;
	struct synaptics_rmi4_f1a_control button_control;
};

#ifdef HTC_FEATURE
struct f34_v7_query_0 {
	union {
		struct {
			unsigned char subpacket_1_size:3;
			unsigned char has_config_id:1;
			unsigned char f34_query0_b4:1;
			unsigned char has_thqa:1;
			unsigned char f34_query0_b6__7:2;
		} __packed;
		unsigned char data[1];
	};
};

struct f34_v7_query_1_7 {
	union {
		struct {
			/* query 1 */
			unsigned char bl_minor_revision;
			unsigned char bl_major_revision;

			/* query 2 */
			unsigned char bl_fw_id_7_0;
			unsigned char bl_fw_id_15_8;
			unsigned char bl_fw_id_23_16;
			unsigned char bl_fw_id_31_24;

			/* query 3 */
			unsigned char minimum_write_size;
			unsigned char block_size_7_0;
			unsigned char block_size_15_8;
			unsigned char flash_page_size_7_0;
			unsigned char flash_page_size_15_8;

			/* query 4 */
			unsigned char adjustable_partition_area_size_7_0;
			unsigned char adjustable_partition_area_size_15_8;

			/* query 5 */
			unsigned char flash_config_length_7_0;
			unsigned char flash_config_length_15_8;

			/* query 6 */
			unsigned char payload_length_7_0;
			unsigned char payload_length_15_8;

			/* query 7 */
			unsigned char f34_query7_b0:1;
			unsigned char has_bootloader:1;
			unsigned char has_device_config:1;
			unsigned char has_flash_config:1;
			unsigned char has_manufacturing_block:1;
			unsigned char has_guest_serialization:1;
			unsigned char has_global_parameters:1;
			unsigned char has_core_code:1;
			unsigned char has_core_config:1;
			unsigned char has_guest_code:1;
			unsigned char has_display_config:1;
			unsigned char f34_query7_b11__15:5;
			unsigned char f34_query7_b16__23;
			unsigned char f34_query7_b24__31;
		} __packed;
		unsigned char data[21];
	};
};

struct synaptics_rmi4_f54_query {
	union {
		struct {
			/* query 0 */
			unsigned char num_of_rx_electrodes;

			/* query 1 */
			unsigned char num_of_tx_electrodes;

			/* query 2 */
			unsigned char f54_query2_b0__1:2;
			unsigned char has_baseline:1;
			unsigned char has_image8:1;
			unsigned char f54_query2_b4__5:2;
			unsigned char has_image16:1;
			unsigned char f54_query2_b7:1;

			/* queries 3.0 and 3.1 */
			unsigned short clock_rate;

			/* query 4 */
			unsigned char touch_controller_family;

			/* query 5 */
			unsigned char has_pixel_touch_threshold_adjustment:1;
			unsigned char f54_query5_b1__7:7;

			/* query 6 */
			unsigned char has_sensor_assignment:1;
			unsigned char has_interference_metric:1;
			unsigned char has_sense_frequency_control:1;
			unsigned char has_firmware_noise_mitigation:1;
			unsigned char has_ctrl11:1;
			unsigned char has_two_byte_report_rate:1;
			unsigned char has_one_byte_report_rate:1;
			unsigned char has_relaxation_control:1;

			/* query 7 */
			unsigned char curve_compensation_mode:2;
			unsigned char f54_query7_b2__7:6;

			/* query 8 */
			unsigned char f54_query8_b0:1;
			unsigned char has_iir_filter:1;
			unsigned char has_cmn_removal:1;
			unsigned char has_cmn_maximum:1;
			unsigned char has_touch_hysteresis:1;
			unsigned char has_edge_compensation:1;
			unsigned char has_per_frequency_noise_control:1;
			unsigned char has_enhanced_stretch:1;

			/* query 9 */
			unsigned char has_force_fast_relaxation:1;
			unsigned char has_multi_metric_state_machine:1;
			unsigned char has_signal_clarity:1;
			unsigned char has_variance_metric:1;
			unsigned char has_0d_relaxation_control:1;
			unsigned char has_0d_acquisition_control:1;
			unsigned char has_status:1;
			unsigned char has_slew_metric:1;

			/* query 10 */
			unsigned char has_h_blank:1;
			unsigned char has_v_blank:1;
			unsigned char has_long_h_blank:1;
			unsigned char has_startup_fast_relaxation:1;
			unsigned char has_esd_control:1;
			unsigned char has_noise_mitigation2:1;
			unsigned char has_noise_state:1;
			unsigned char has_energy_ratio_relaxation:1;

			/* query 11 */
			unsigned char has_excessive_noise_reporting:1;
			unsigned char has_slew_option:1;
			unsigned char has_two_overhead_bursts:1;
			unsigned char has_query13:1;
			unsigned char has_one_overhead_burst:1;
			unsigned char f54_query11_b5:1;
			unsigned char has_ctrl88:1;
			unsigned char has_query15:1;

			/* query 12 */
			unsigned char number_of_sensing_frequencies:4;
			unsigned char f54_query12_b4__7:4;
		} __packed;
		unsigned char data[14];
	};
};

struct synaptics_rmi4_f54_query_13 {
	union {
		struct {
			unsigned char has_ctrl86:1;
			unsigned char has_ctrl87:1;
			unsigned char has_ctrl87_sub0:1;
			unsigned char has_ctrl87_sub1:1;
			unsigned char has_ctrl87_sub2:1;
			unsigned char has_cidim:1;
			unsigned char has_noise_mitigation_enhancement:1;
			unsigned char has_rail_im:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_15 {
	union {
		struct {
			unsigned char has_ctrl90:1;
			unsigned char has_transmit_strength:1;
			unsigned char has_ctrl87_sub3:1;
			unsigned char has_query16:1;
			unsigned char has_query20:1;
			unsigned char has_query21:1;
			unsigned char has_query22:1;
			unsigned char has_query25:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_16 {
	union {
		struct {
			unsigned char has_query17:1;
			unsigned char has_data17:1;
			unsigned char has_ctrl92:1;
			unsigned char has_ctrl93:1;
			unsigned char has_ctrl94_query18:1;
			unsigned char has_ctrl95_query19:1;
			unsigned char has_ctrl99:1;
			unsigned char has_ctrl100:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_21 {
	union {
		struct {
			unsigned char has_abs_rx:1;
			unsigned char has_abs_tx:1;
			unsigned char has_ctrl91:1;
			unsigned char has_ctrl96:1;
			unsigned char has_ctrl97:1;
			unsigned char has_ctrl98:1;
			unsigned char has_data19:1;
			unsigned char has_query24_data18:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_22 {
	union {
		struct {
			unsigned char has_packed_image:1;
			unsigned char has_ctrl101:1;
			unsigned char has_dynamic_sense_display_ratio:1;
			unsigned char has_query23:1;
			unsigned char has_ctrl103_query26:1;
			unsigned char has_ctrl104:1;
			unsigned char has_ctrl105:1;
			unsigned char has_query28:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_23 {
	union {
		struct {
			unsigned char has_ctrl102:1;
			unsigned char has_ctrl102_sub1:1;
			unsigned char has_ctrl102_sub2:1;
			unsigned char has_ctrl102_sub4:1;
			unsigned char has_ctrl102_sub5:1;
			unsigned char has_ctrl102_sub9:1;
			unsigned char has_ctrl102_sub10:1;
			unsigned char has_ctrl102_sub11:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_25 {
	union {
		struct {
			unsigned char has_ctrl106:1;
			unsigned char has_ctrl102_sub12:1;
			unsigned char has_ctrl107:1;
			unsigned char has_ctrl108:1;
			unsigned char has_ctrl109:1;
			unsigned char has_data20:1;
			unsigned char f54_query25_b6:1;
			unsigned char has_query27:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_27 {
	union {
		struct {
			unsigned char has_ctrl110:1;
			unsigned char has_data21:1;
			unsigned char has_ctrl111:1;
			unsigned char has_ctrl112:1;
			unsigned char has_ctrl113:1;
			unsigned char has_data22:1;
			unsigned char has_ctrl114:1;
			unsigned char has_query29:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_29 {
	union {
		struct {
			unsigned char has_ctrl115:1;
			unsigned char has_ground_ring_options:1;
			unsigned char has_lost_bursts_tuning:1;
			unsigned char has_aux_exvcom2_select:1;
			unsigned char has_ctrl116:1;
			unsigned char has_data23:1;
			unsigned char has_ctrl117:1;
			unsigned char has_query30:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_30 {
	union {
		struct {
			unsigned char has_ctrl118:1;
			unsigned char has_ctrl119:1;
			unsigned char has_ctrl120:1;
			unsigned char has_ctrl121:1;
			unsigned char has_ctrl122_query31:1;
			unsigned char has_ctrl123:1;
			unsigned char has_ctrl124:1;
			unsigned char has_query32:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_32 {
	union {
		struct {
			unsigned char has_ctrl125:1;
			unsigned char has_ctrl126:1;
			unsigned char has_ctrl127:1;
			unsigned char has_abs_charge_pump_disable:1;
			unsigned char has_query33:1;
			unsigned char has_data24:1;
			unsigned char has_query34:1;
			unsigned char has_query35:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_33 {
	union {
		struct {
			unsigned char has_ctrl128:1;
			unsigned char has_ctrl129:1;
			unsigned char has_ctrl130:1;
			unsigned char has_ctrl131:1;
			unsigned char has_ctrl132:1;
			unsigned char has_ctrl133:1;
			unsigned char has_ctrl134:1;
			unsigned char has_query36:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_35 {
	union {
		struct {
			unsigned char has_data25:1;
			unsigned char has_ctrl135:1;
			unsigned char has_ctrl136:1;
			unsigned char has_ctrl137:1;
			unsigned char has_ctrl138:1;
			unsigned char has_ctrl139:1;
			unsigned char has_data26:1;
			unsigned char has_ctrl140:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_36 {
	union {
		struct {
			unsigned char has_ctrl141:1;
			unsigned char has_ctrl142:1;
			unsigned char has_query37:1;
			unsigned char has_ctrl143:1;
			unsigned char has_ctrl144:1;
			unsigned char has_ctrl145:1;
			unsigned char has_ctrl146:1;
			unsigned char has_query38:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_38 {
	union {
		struct {
			unsigned char has_ctrl147:1;
			unsigned char has_ctrl148:1;
			unsigned char has_ctrl149:1;
			unsigned char has_ctrl150:1;
			unsigned char has_ctrl151:1;
			unsigned char has_ctrl152:1;
			unsigned char has_ctrl153:1;
			unsigned char has_query39:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_39 {
	union {
		struct {
			unsigned char has_ctrl154:1;
			unsigned char has_ctrl155:1;
			unsigned char has_ctrl156:1;
			unsigned char has_ctrl160:1;
			unsigned char has_ctrl157_ctrl158:1;
			unsigned char f54_query39_b5__6:2;
			unsigned char has_query40:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_40 {
	union {
		struct {
			unsigned char has_ctrl169:1;
			unsigned char has_ctrl163_query41:1;
			unsigned char f54_query40_b2:1;
			unsigned char has_ctrl165_query42:1;
			unsigned char has_ctrl166:1;
			unsigned char has_ctrl167:1;
			unsigned char has_ctrl168:1;
			unsigned char has_query43:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_43 {
	union {
		struct {
			unsigned char f54_query43_b0__1:2;
			unsigned char has_ctrl171:1;
			unsigned char has_ctrl172_query44_query45:1;
			unsigned char has_ctrl173:1;
			unsigned char has_ctrl174:1;
			unsigned char has_ctrl175:1;
			unsigned char has_query46:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_46 {
	union {
		struct {
			unsigned char has_ctrl176:1;
			unsigned char has_ctrl177_ctrl178:1;
			unsigned char has_ctrl179:1;
			unsigned char f54_query46_b3:1;
			unsigned char has_data27:1;
			unsigned char has_data28:1;
			unsigned char f54_query46_b6:1;
			unsigned char has_query47:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_47 {
	union {
		struct {
			unsigned char f54_query47_b0:1;
			unsigned char has_ctrl182:1;
			unsigned char has_ctrl183:1;
			unsigned char f54_query47_b3:1;
			unsigned char has_ctrl185:1;
			unsigned char has_ctrl186:1;
			unsigned char has_ctrl187:1;
			unsigned char has_query49:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_49 {
	union {
		struct {
			unsigned char f54_query49_b0__1:2;
			unsigned char has_ctrl188:1;
			unsigned char has_data31:1;
			unsigned char f54_query49_b4__6:3;
			unsigned char has_query50:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_50 {
	union {
		struct {
			unsigned char f54_query50_b0__6:7;
			unsigned char has_query51:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_51 {
	union {
		struct {
			unsigned char f54_query51_b0__4:5;
			unsigned char has_query53_query54_ctrl198:1;
			unsigned char has_ctrl199:1;
			unsigned char has_query55:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_55 {
	union {
		struct {
			unsigned char has_query56:1;
			unsigned char has_data33_data34:1;
			unsigned char has_alt_report_rate:1;
			unsigned char has_ctrl200:1;
			unsigned char has_ctrl201_ctrl202:1;
			unsigned char has_ctrl203:1;
			unsigned char has_ctrl204:1;
			unsigned char has_query57:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_57 {
	union {
		struct {
			unsigned char has_ctrl205:1;
			unsigned char has_ctrl206:1;
			unsigned char has_usb_bulk_read:1;
			unsigned char has_ctrl207:1;
			unsigned char has_ctrl208:1;
			unsigned char has_ctrl209:1;
			unsigned char has_ctrl210:1;
			unsigned char has_query58:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_58 {
	union {
		struct {
			unsigned char has_query59:1;
			unsigned char has_query60:1;
			unsigned char has_ctrl211:1;
			unsigned char has_ctrl212:1;
			unsigned char has_hybrid_abs_tx_axis_filtering:1;
			unsigned char has_hybrid_abs_tx_interpolation:1;
			unsigned char has_ctrl213:1;
			unsigned char has_query61:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_61 {
	union {
		struct {
			unsigned char has_ctrl214:1;
			unsigned char has_ctrl215_query62_query63:1;
			unsigned char f54_query_61_b2:1;
			unsigned char has_ctrl216:1;
			unsigned char has_ctrl217:1;
			unsigned char has_misc_host_ctrl:1;
			unsigned char hybrid_abs_buttons:1;
			unsigned char has_query64:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_64 {
	union {
		struct {
			unsigned char has_ctrl101_sub1:1;
			unsigned char has_ctrl220:1;
			unsigned char has_ctrl221:1;
			unsigned char has_ctrl222:1;
			unsigned char has_ctrl219_sub1:1;
			unsigned char has_ctrl103_sub3:1;
			unsigned char has_ctrl224_ctrl226_ctrl227:1;
			unsigned char has_query65:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_65 {
	union {
		struct {
			unsigned char f54_query_65_b0__1:2;
			unsigned char has_ctrl101_sub2:1;
			unsigned char f54_query_65_b3__4:2;
			unsigned char has_query66_ctrl231:1;
			unsigned char has_ctrl232:1;
			unsigned char has_query67:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_67 {
	union {
		struct {
			unsigned char has_abs_doze_spatial_filter_en:1;
			unsigned char has_abs_doze_avg_filter_enhancement_en:1;
			unsigned char has_single_display_pulse:1;
			unsigned char f54_query_67_b3__4:2;
			unsigned char has_ctrl235_ctrl236:1;
			unsigned char f54_query_67_b6:1;
			unsigned char has_query68:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f54_query_68 {
	union {
		struct {
			unsigned char f54_query_68_b0:1;
			unsigned char has_ctrl238:1;
			unsigned char has_ctrl238_sub1:1;
			unsigned char has_ctrl238_sub2:1;
			unsigned char has_ctrl239:1;
			unsigned char has_freq_filter_bw_ext:1;
			unsigned char is_tddi_hic:1;
			unsigned char has_query69:1;
		} __packed;
		unsigned char data[1];
	};
};

struct synaptics_rmi4_f55_query {
	union {
		struct {
			/* query 0 */
			unsigned char num_of_rx_electrodes;

			/* query 1 */
			unsigned char num_of_tx_electrodes;

			/* query 2 */
			unsigned char has_sensor_assignment:1;
			unsigned char has_edge_compensation:1;
			unsigned char curve_compensation_mode:2;
			unsigned char has_ctrl6:1;
			unsigned char has_alternate_transmitter_assignment:1;
			unsigned char has_single_layer_multi_touch:1;
			unsigned char has_query5:1;
		} __packed;
		unsigned char data[3];
	};
};

#endif

struct synaptics_rmi4_exp_fhandler {
	struct synaptics_rmi4_exp_fn *exp_fn;
	bool insert;
	bool remove;
	struct list_head link;
};

struct synaptics_rmi4_exp_fn_data {
	bool initialized;
	bool queue_work;
	struct mutex mutex;
	struct list_head list;
	struct delayed_work work;
	struct workqueue_struct *workqueue;
	struct synaptics_rmi4_data *rmi4_data;
};

static struct synaptics_rmi4_exp_fn_data exp_data;

static struct synaptics_dsx_button_map *vir_button_map;

#ifdef USE_DATA_SERVER
static pid_t synad_pid;
static struct task_struct *synad_task;
static struct siginfo interrupt_signal;
#endif

#ifdef HTC_FEATURE
#ifdef CONFIG_TOUCHSCREEN_USE_DSI_NOTIFY
static struct dsi_status_notifier dsi_event_notifier = {
	.name = "synaptics_dsi_event_handler",
	.func = dsi_status_detect,
};
#endif
#endif

static struct device_attribute attrs[] = {
	__ATTR(reset, (S_IWUSR | S_IWGRP),
			synaptics_rmi4_show_error,
			synaptics_rmi4_f01_reset_store),
	__ATTR(productinfo, S_IRUGO,
			synaptics_rmi4_f01_productinfo_show,
			synaptics_rmi4_store_error),
	__ATTR(buildid, S_IRUGO,
			synaptics_rmi4_f01_buildid_show,
			synaptics_rmi4_store_error),
	__ATTR(flashprog, S_IRUGO,
			synaptics_rmi4_f01_flashprog_show,
			synaptics_rmi4_store_error),
	__ATTR(0dbutton, (S_IRUGO | S_IWUSR | S_IWGRP),
			synaptics_rmi4_0dbutton_show,
			synaptics_rmi4_0dbutton_store),
	__ATTR(suspend, (S_IWUSR | S_IWGRP),
			synaptics_rmi4_show_error,
			synaptics_rmi4_suspend_store),
	__ATTR(wake_gesture, (S_IRUGO | S_IWUSR | S_IWGRP),
			synaptics_rmi4_wake_gesture_show,
			synaptics_rmi4_wake_gesture_store),
#ifdef USE_DATA_SERVER
	__ATTR(synad_pid, (S_IWUSR | S_IWGRP),
			synaptics_rmi4_show_error,
			synaptics_rmi4_synad_pid_store),
#endif
#if defined(CONFIG_SECURE_TOUCH)
        __ATTR(secure_touch_enable, (S_IRUGO | S_IWUSR | S_IWGRP),
                        synaptics_secure_touch_enable_show,
                        synaptics_secure_touch_enable_store),
        __ATTR(secure_touch, S_IRUGO ,
                        synaptics_secure_touch_show,
                        NULL),
#endif
};

static struct kobj_attribute virtual_key_map_attr = {
	.attr = {
		.name = VIRTUAL_KEY_MAP_FILE_NAME,
		.mode = S_IRUGO,
	},
	.show = synaptics_rmi4_virtual_key_map_show,
};

#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
void lcm_rst_callback_register (void (*lcm_rst_func)(void))
{
	pr_info("%s: ++\n", __func__);
	set_lcm_rst = lcm_rst_func;
}
EXPORT_SYMBOL(lcm_rst_callback_register);
#endif

static ssize_t synaptics_rmi4_f01_reset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int retval;
	unsigned int reset;
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	if (sscanf(buf, "%u", &reset) != 1)
		return -EINVAL;

	if (reset != 1)
		return -EINVAL;

	retval = synaptics_rmi4_reset_device(rmi4_data, false);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to issue reset command, error = %d\n",
				__func__, retval);
		return retval;
	}

	return count;
}

static ssize_t synaptics_rmi4_f01_productinfo_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "0x%02x 0x%02x\n",
			(rmi4_data->rmi4_mod_info.product_info[0]),
			(rmi4_data->rmi4_mod_info.product_info[1]));
}

static ssize_t synaptics_rmi4_f01_buildid_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n",
			rmi4_data->firmware_id);
}

static ssize_t synaptics_rmi4_f01_flashprog_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int retval;
	struct synaptics_rmi4_f01_device_status device_status;
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_data_base_addr,
			device_status.data,
			sizeof(device_status.data));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read device status, error = %d\n",
				__func__, retval);
		return retval;
	}

	return snprintf(buf, PAGE_SIZE, "%u\n",
			device_status.flash_prog);
}

static ssize_t synaptics_rmi4_0dbutton_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n",
			rmi4_data->button_0d_enabled);
}

static ssize_t synaptics_rmi4_0dbutton_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int retval;
	unsigned int input;
	unsigned char ii;
	unsigned char intr_enable;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);
	struct synaptics_rmi4_device_info *rmi;

	rmi = &(rmi4_data->rmi4_mod_info);

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	input = input > 0 ? 1 : 0;

	if (rmi4_data->button_0d_enabled == input)
		return count;

	if (list_empty(&rmi->support_fn_list))
		return -ENODEV;

	list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
		if (fhandler->fn_number == SYNAPTICS_RMI4_F1A) {
			ii = fhandler->intr_reg_num;

			retval = synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f01_ctrl_base_addr + 1 + ii,
					&intr_enable,
					sizeof(intr_enable));
			if (retval < 0)
				return retval;

			if (input == 1)
				intr_enable |= fhandler->intr_mask;
			else
				intr_enable &= ~fhandler->intr_mask;

			retval = synaptics_rmi4_reg_write(rmi4_data,
					rmi4_data->f01_ctrl_base_addr + 1 + ii,
					&intr_enable,
					sizeof(intr_enable));
			if (retval < 0)
				return retval;
		}
	}

	rmi4_data->button_0d_enabled = input;

	return count;
}

static ssize_t synaptics_rmi4_suspend_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int input;

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	if (input == 1)
		synaptics_rmi4_suspend(dev);
	else if (input == 0)
		synaptics_rmi4_resume(dev);
	else
		return -EINVAL;

	return count;
}

static ssize_t synaptics_rmi4_wake_gesture_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n",
			rmi4_data->enable_wakeup_gesture);
}

static ssize_t synaptics_rmi4_wake_gesture_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int input;
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	input = input > 0 ? 1 : 0;

	if (rmi4_data->f11_wakeup_gesture || rmi4_data->f12_wakeup_gesture)
		rmi4_data->enable_wakeup_gesture = input;

	return count;
}

#ifdef HTC_FEATURE
static ssize_t touch_detect_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;

	if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG) {
		pr_info("%s: %d\n", __func__, rmi4_data->touch_detect);
	}
	return snprintf(buf, PAGE_SIZE, "%d\n", rmi4_data->touch_detect);
}

static ssize_t synaptics_reset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	ssize_t retval = 0;
	unsigned int input;

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	if (input == 1) {
		retval = synaptics_rmi4_hw_reset(rmi4_data);
		if (retval < 0)
			return retval;
	} else if (input == 2) {
		retval = synaptics_rmi4_reset_device(rmi4_data, false);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: SW reset failed, error = %ld\n", __func__, retval);
			return retval;
		}
		dev_info(rmi4_data->pdev->dev.parent, "%s: SW reset\n", __func__);
	} else
		return -EINVAL;

	return count;
}

static ssize_t synaptics_debug_show(struct device *dev, struct device_attribute *attr,
	char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	return snprintf(buf, PAGE_SIZE, "%08X\n", rmi4_data->debug_mask);
}

static ssize_t synaptics_debug_store(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t count)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;

	if (sscanf(buf, "%ux", &rmi4_data->debug_mask) != 1)
		return -EINVAL;

	return count;
}

static ssize_t touch_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	ssize_t count = 0;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	count = snprintf(buf, PAGE_SIZE, "synaptics-%d", rmi4_data->chip_id);
	if (bdata->panel_id_num != 0)
		count += scnprintf(buf + count, PAGE_SIZE, "_panelID-0x%x", rmi4_data->panel_ID);
	count += scnprintf(buf + count, PAGE_SIZE, "_PR:%d\n", rmi4_data->firmware_id);

	return count;
}

static ssize_t touch_config_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;

	return snprintf(buf, PAGE_SIZE, "config_id: %s\n", rmi4_data->config_id);
}

static int synaptics_read_diag_data(struct synaptics_rmi4_data *rmi4_data)
{
	unsigned char cmd = 0x01;
	int retval = 0;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f54_data_base_addr,
			&rmi4_data->diag_command,
			sizeof(rmi4_data->diag_command));
	if (retval < 0) {
		dev_info(rmi4_data->pdev->dev.parent," %s: Failed to write report type\n", __func__);
		return retval;
	}
	atomic_set(&rmi4_data->data_ready, 0);

	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f54_cmd_base_addr,
			&cmd,
			sizeof(cmd));
	if (retval < 0) {
		atomic_set(&rmi4_data->data_ready, 1);
		dev_err(rmi4_data->pdev->dev.parent," %s: Failed to write get report command\n", __func__);
		return retval;
	}

	wait_event_interruptible_timeout(syn_data_ready_wq,
			atomic_read(&rmi4_data->data_ready), 50);

	//Read the self-capacity data
	switch (rmi4_data->diag_command) {
		case F54_16BIT_IMAGE:
			rmi4_data->diag_command = F54_ABS_HYBRID_DELTA_CAP;
			retval = synaptics_read_diag_data(rmi4_data);
			break;
		case F54_RAW_16BIT_IMAGE:
			rmi4_data->diag_command = F54_ABS_HYBRID_RAW_CAP;
			retval = synaptics_read_diag_data(rmi4_data);
			break;
		case F54_ABS_HYBRID_DELTA_CAP:
			rmi4_data->diag_command = F54_16BIT_IMAGE;
			break;
		case F54_ABS_HYBRID_RAW_CAP:
			rmi4_data->diag_command = F54_RAW_16BIT_IMAGE;
			break;
		default:
			dev_err(rmi4_data->pdev->dev.parent," %s: Incorrect diag command\n", __func__);
			break;
	}

	return retval;
}

static ssize_t synaptics_diag_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	ssize_t count = 0;
	int retval;
	uint8_t i, j, x, y;

	if (rmi4_data->report_data == NULL || rmi4_data->report_data_32 == NULL)
		return -EINVAL;

	if (rmi4_data->num_of_tx == 0 || rmi4_data->num_of_rx == 0)
		return -EINVAL;

	retval = synaptics_read_diag_data(rmi4_data);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent," %s: Failed to get diag data\n", __func__);
		return retval;
	}

	if (rmi4_data->num_of_tx > rmi4_data->num_of_rx) {
		x = rmi4_data->num_of_tx;
		y = rmi4_data->num_of_rx;
	} else {
		x = rmi4_data->num_of_rx;
		y = rmi4_data->num_of_tx;
	}

	switch (rmi4_data->chip_id) {
		case 2200:
		case 3202:
		case 4353:
		case 7508:
			for (i = 0; i < x; i++) {
				for (j = 0; j < y; j++) {
					count += snprintf(buf + count, PAGE_SIZE, "%5d", rmi4_data->report_data[i + j*x]);
				}
				count += snprintf(buf + count, PAGE_SIZE, "\t  %5d\n\n", rmi4_data->report_data_32[i]);
			}
			count += snprintf(buf + count, PAGE_SIZE, "\n");
			for (i = 0; i < y; i+=2)
				count += snprintf(buf + count, PAGE_SIZE, "%5d     ", rmi4_data->report_data_32[x+i]);
			count += snprintf(buf + count, PAGE_SIZE, "\n");
			for (i = 1; i < y; i+=2)
				count += snprintf(buf + count, PAGE_SIZE, "     %5d", rmi4_data->report_data_32[x+i]);
			count += snprintf(buf + count, PAGE_SIZE, "\n");
			break;
		//case 3201:
		//case 3351:
		//case 3508:
		//case 3528:
		//case 3708:
		//case 3718:
		default:
			for (i = 0; i < x; i++) {
				for (j = 0; j < y; j++) {
					count += snprintf(buf + count, PAGE_SIZE, "%5d", rmi4_data->report_data[i*y + j]);
				}
				count += snprintf(buf + count, PAGE_SIZE, "\t  %5d\n\n", rmi4_data->report_data_32[y+i]);
			}
			count += snprintf(buf + count, PAGE_SIZE, "\n");
			for (i = 0; i < y; i+=2)
				count += snprintf(buf + count, PAGE_SIZE, "%5d     ", rmi4_data->report_data_32[i]);
			count += snprintf(buf + count, PAGE_SIZE, "\n");
			for (i = 1; i < y; i+=2)
				count += snprintf(buf + count, PAGE_SIZE, "     %5d", rmi4_data->report_data_32[i]);
			count += snprintf(buf + count, PAGE_SIZE, "\n");
			break;
	}

	return count;
}

static ssize_t synaptics_diag_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	unsigned int input;

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	if (input == 1)
		rmi4_data->diag_command = F54_16BIT_IMAGE;
	else if (input == 2)
		rmi4_data->diag_command = F54_RAW_16BIT_IMAGE;
	else
		return -EINVAL;

	return count;
}

static ssize_t int_status_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	size_t count = 0;

	count = snprintf(buf, PAGE_SIZE, "%d\n", rmi4_data->irq_enabled);

	return count;
}

static ssize_t int_status_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	unsigned int input;

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	mutex_lock(&(rmi4_data->rmi4_irq_enable_mutex));

	if (input == 0) {
		disable_irq(rmi4_data->irq);
		rmi4_data->irq_enabled = false;
	} else if (input == 1) {
		enable_irq(rmi4_data->irq);
		rmi4_data->irq_enabled = true;
	} else
		return -EINVAL;

	dev_info(rmi4_data->pdev->dev.parent, "%s: interrupt %s\n",
			__func__, (rmi4_data->irq_enabled) ? "enable" : "disable");

	mutex_unlock(&(rmi4_data->rmi4_irq_enable_mutex));

	return count;
}

static void synaptics_set_pmic(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_device_info *rmi;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;
	unsigned short i2c_addr = rmi4_data->f51_ctrl_base_addr + bdata->f51_set_pmic_offset;
	unsigned char i2c_value = enable;
	int retval;

	if (bdata->f51_set_pmic_offset < 0) {
		pr_err("%s: Unsupport offset %d\n", __func__, bdata->f51_set_pmic_offset);
		return;
	}

	if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
		pr_info("%s %s\n", __func__, enable ? "enable" : "disable");

	rmi = &(rmi4_data->rmi4_mod_info);

	list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
		if (fhandler->fn_number == SYNAPTICS_RMI4_F51)
			break;
	}

	if (fhandler->fn_number != SYNAPTICS_RMI4_F51) {
		pr_err("%s: Not support F51\n", __func__);
		return;
	}

	retval = synaptics_rmi4_reg_write(rmi4_data,
			i2c_addr,
			&i2c_value,
			sizeof(i2c_value));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to set pmic %s\n",
				__func__, enable ? "on" : "off");
		return;
	}

	return;
}

static DEVICE_ATTR(reset, S_IWUSR, NULL, synaptics_reset_store);
static DEVICE_ATTR(debug_level, (S_IWUSR|S_IRUGO), synaptics_debug_show, synaptics_debug_store);
static DEVICE_ATTR(touch_detect, S_IRUGO, touch_detect_show, NULL);
static DEVICE_ATTR(vendor, S_IRUGO, touch_vendor_show, NULL);
static DEVICE_ATTR(config, S_IRUGO, touch_config_show, NULL);
static DEVICE_ATTR(diag, (S_IWUSR | S_IRUGO), synaptics_diag_show, synaptics_diag_store);
static DEVICE_ATTR(enabled, (S_IWUSR|S_IRUGO), int_status_show, int_status_store);

static int synaptics_rmi4_set_mode_reg(struct synaptics_rmi4_data *rmi4_data)
{
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;
	struct synaptics_rmi4_f12_extra_data *f12_extra_data = NULL;
	struct synaptics_rmi4_fn *f12_fhandler = NULL;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_device_info *rmi;
	unsigned short i2c_addr = 0;
	unsigned short data_size = 0;
	unsigned char read_data[MAX_SET_MODE_REG_SIZE] = {0};
	unsigned char i2c_value[MAX_SET_MODE_REG_SIZE] = {0};
	bool need_change[MAX_SET_MODE_REG_SIZE] = {0};
	bool support_f51 = false, support_f54 = false;
	int retval = 0, i, data_index;

	pr_info("%s\n", __func__);

	rmi = &(rmi4_data->rmi4_mod_info);

	list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
		if (fhandler->fn_number == SYNAPTICS_RMI4_F12) {
			f12_fhandler = fhandler;
			f12_extra_data = (struct synaptics_rmi4_f12_extra_data *)f12_fhandler->extra;
		} else if (fhandler->fn_number == SYNAPTICS_RMI4_F51) {
			support_f51 = true;
		} else if (fhandler->fn_number == SYNAPTICS_RMI4_F54) {
			support_f54 = true;
		}
	}

//	if (bdata->support_cover && rmi4_data->sensitivity_setting & COVER_MODE_BIT) {
//
// TBD: cover mode
//
//	} else if (bdata->support_glove) {
	if (bdata->support_glove) {
		if (bdata->glove_cmd_num == 0 || bdata->glove_cmd_num > MAX_MODE_CMD_NUM) {
			pr_err("%s: incorrect command number %d\n", __func__, bdata->glove_cmd_num);
			return -EINVAL;
		}
		for (i = 0; i < bdata->glove_cmd_num; i++) {
			/* F12_2D_CTRL10(00)/00 */
			if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL10_00_00") == 0) {
				if (!f12_extra_data || !f12_fhandler) {
					pr_err("%s: failed to set F12_2D_CTRL10_00_00, F12 not found\n", __func__);
					return -ENXIO;
				} else if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL10_00_00\n", __func__);
				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl10_offset;
				i2c_value[0] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[0] = true;
				data_size = 1;

				if ((i+1) < bdata->glove_cmd_num &&
						/* check next: F12_2D_CTRL10 */
						(strncmp(bdata->glove_reg_name[i+1], bdata->glove_reg_name[i], 13)) == 0) {
					continue;
				}
			/* F12_2D_CTRL10(00)/01 */
			} else if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL10_00_01") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F12_2D_CTRL10_00_01, F12 not found\n", __func__);
					return -ENXIO;
				} else if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL10_00_01\n", __func__);
				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl10_offset;
				i2c_value[1] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[1] = true;
				data_size = 2;

				if ((i+1) < bdata->glove_cmd_num &&
						/* check next: F12_2D_CTRL10 */
						(strncmp(bdata->glove_reg_name[i+1], bdata->glove_reg_name[i], 13)) == 0) {
					continue;
				}

			/* F12_2D_CTRL10(02)/00 */
			} else if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL10_02_00") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F12_2D_CTRL10_02_00, F12 not found\n", __func__);
					return -ENXIO;
				} else if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL10_02_00\n", __func__);
				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl10_offset;
				i2c_value[5] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[5] = true;
				data_size = 6;

			/* F12_2D_CTRL15(00)/00 */
			} else if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL15_00_00") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F12_2D_CTRL15_00_00, F12 not found\n", __func__);
					return -ENXIO;
				} else if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL15_00_00\n", __func__);
				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl15_offset;
				i2c_value[0] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[0] = true;
				data_size = 1;

				if ((i+1) < bdata->glove_cmd_num &&
						/* check next: F12_2D_CTRL15 */
						(strncmp(bdata->glove_reg_name[i+1], bdata->glove_reg_name[i], 13)) == 0) {
					continue;
				}

			/* F12_2D_CTRL15(00)/01 */
			} else if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL15_00_01") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F12_2D_CTRL15_00_01, F12 not found\n", __func__);
					return -ENXIO;
				} else if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL15_00_01\n", __func__);

				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl15_offset;
				i2c_value[1] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[1] = true;
				data_size = 2;

			/* F12_2D_CTRL23(00)/00 */
			} else if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL23_00_00") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F12_2D_CTRL23_00_00, F12 not found\n", __func__);
					return -ENXIO;
				}
				if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL23_00_00\n", __func__);

				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl23_offset;
				i2c_value[0] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[0] = true;
				data_size = 1;

			/* F12_2D_CTRL56(00)/00 */
			} else if (strcmp(bdata->glove_reg_name[i], "F12_2D_CTRL56_00_00") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F12_2D_CTRL56_00_00, F12 not found\n", __func__);
					return -ENXIO;
				}
				if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F12_2D_CTRL56_00_00\n", __func__);

				i2c_addr = f12_fhandler->full_addr.ctrl_base + f12_extra_data->ctrl56_offset;
				i2c_value[0] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[0] = true;
				data_size = 1;

			/* F55_SENSOR_CTRL03(00)/02 */
			} else if (strcmp(bdata->glove_reg_name[i], "F55_SENSOR_CTRL03_00_02") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F55_SENSOR_CTRL03_00_02, F55 not found\n", __func__);
					return -ENXIO;
				}
				if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F55_SENSOR_CTRL03_00_02\n", __func__);

				i2c_addr = rmi4_data->f55_ctrl_base_addr + rmi4_data->f55_ctrl03_offset;
				i2c_value[2] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[2] = true;
				data_size = 3;

				if ((i+1) < bdata->glove_cmd_num &&
						/* check next: F55_SENSOR_CTRL03 */
						(strncmp(bdata->glove_reg_name[i+1], bdata->glove_reg_name[i], 17)) == 0) {
					continue;
				}

			/* F55_SENSOR_CTRL03(00)/03 */
			} else if (strcmp(bdata->glove_reg_name[i], "F55_SENSOR_CTRL03_00_03") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F55_SENSOR_CTRL03_00_03, F55 not found\n", __func__);
					return -ENXIO;
				}
				if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F55_SENSOR_CTRL03_00_03\n", __func__);

				i2c_addr = rmi4_data->f55_ctrl_base_addr + rmi4_data->f55_ctrl03_offset;
				i2c_value[3] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[3] = true;
				data_size = 4;

			/* F54_ANALOG_CTRL113(00)/03 */
			}  else if (strcmp(bdata->glove_reg_name[i], "F54_ANALOG_CTRL113_00_03") == 0) {
				if (!f12_extra_data) {
					pr_err("%s: failed to set F54_ANALOG_CTRL113_00_03, F54 not found\n", __func__);
					return -ENXIO;
				}
				if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: F54_ANALOG_CTRL113_00_03\n", __func__);

				i2c_addr = rmi4_data->f54_ctrl113_address;
				i2c_value[3] = rmi4_data->glove_setting ? bdata->glove_reg[i] : bdata->normal_reg[i];
				need_change[3] = true;
				data_size = 4;

			} else {
				pr_err("%s: unsupport register\n", __func__);
				return -ENXIO;
			}

			if (data_size > 0) {
				retval = synaptics_rmi4_reg_read(rmi4_data, i2c_addr, read_data, data_size);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to read 0x%x, size %d\n", __func__, i2c_addr, data_size);
					return -EIO;
				}
				for (data_index = 0; data_index < data_size; data_index++) {
					if (need_change[data_index]) {
						if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
							pr_info("%s: set data[%d] from 0x%x to 0x%x\n",
									__func__,
									data_index,
									read_data[data_index],
									i2c_value[data_index]);
						read_data[data_index] = i2c_value[data_index];
						need_change[data_index] = 0;
					}
				}
				if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
					pr_info("%s: write data to addr:0x%x, length:%d\n",
							__func__,
							i2c_addr,
							data_size);
				retval = synaptics_rmi4_reg_write(rmi4_data, i2c_addr, read_data, data_size);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to write 0x%x, size %d\n", __func__, i2c_addr, data_size);
					return -EIO;
				}
			}
		}

		if (support_f54 && bdata->mode_f54_force_update) {
			i2c_value[0] = F54_CMD_FORCE_UPDATE;
			if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
				pr_info("%s: force-update - write 0x%x to addr 0x%x\n",
						__func__,
						i2c_value[0],
						rmi4_data->f54_cmd_base_addr);
			retval = synaptics_rmi4_reg_write(rmi4_data,
					rmi4_data->f54_cmd_base_addr,
					i2c_value,
					F54_CMD_FORCE_UPDATE_SIZE);
			if (retval < 0) {
				dev_err(rmi4_data->pdev->dev.parent,
						"%s: Failed to write 0x%x, size %d\n", __func__, i2c_addr, data_size);
				return -EIO;
			}
		}
	}

	return retval;
}

static ssize_t synaptics_rmi4_glove_setting_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (!bdata->support_glove)
		return snprintf(buf, PAGE_SIZE, "0\n");

	return snprintf(buf, PAGE_SIZE, "%d\n", rmi4_data->glove_setting);
}

static ssize_t synaptics_rmi4_glove_setting_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	unsigned int input;

	if (sscanf(buf, "%u", &input) != 1) {
		pr_err("bad parameter\n");
		return -EINVAL;
	}

	if (input > 3) {
		pr_info("%s: wrong parameter (%u)\n", __func__, input);
		return -EINVAL;
	}

	rmi4_data->glove_setting = input;
	synaptics_rmi4_set_mode_reg(rmi4_data);
	pr_info("%s: glove_setting change to %d\n", __func__, rmi4_data->glove_setting);

	return count;
}

static DEVICE_ATTR(glove_setting, (S_IWUSR | S_IRUGO),
	synaptics_rmi4_glove_setting_show, synaptics_rmi4_glove_setting_store);

static struct attribute *htc_tp_attrs[] = {
	&dev_attr_reset.attr,
	&dev_attr_debug_level.attr,
	&dev_attr_touch_detect.attr,
	&dev_attr_vendor.attr,
	&dev_attr_config.attr,
	&dev_attr_diag.attr,
	&dev_attr_enabled.attr,
	NULL
};

static const struct attribute_group attr_group = {
	.attrs = htc_tp_attrs,
};

static struct kobject *android_touch_kobj;
static int synaptics_rmi4_sysfs_init(struct synaptics_rmi4_data *rmi4_data, bool enable)
{
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (enable) {
		android_touch_kobj = kobject_create_and_add("android_touch", NULL);
		if (android_touch_kobj == NULL) {
			dev_err(rmi4_data->pdev->dev.parent,"%s: subsystem_register failed\n", __func__);
			return -ENOMEM;
		}

		if (sysfs_create_link(android_touch_kobj, &rmi4_data->input_dev->dev.kobj, "synaptics_rmi4_dsx") < 0) {
			dev_err(rmi4_data->pdev->dev.parent, "%s: failed to create link\n", __func__);
			return -ENOMEM;
		}

		if (sysfs_create_group(android_touch_kobj, &attr_group) < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to create sysfs attributes\n",
					__func__);
			return -ENODEV;
		}

		if (bdata->support_glove) {
			if (sysfs_create_file(android_touch_kobj, &dev_attr_glove_setting.attr) < 0) {
				dev_err(rmi4_data->pdev->dev.parent,
						"%s: Failed to create sysfs attribute [glove]\n",
						__func__);
				return -ENODEV;
			}
		}
	} else {
		sysfs_remove_link(android_touch_kobj, "synaptics_rmi4_dsx");
		if (bdata->support_glove)
			sysfs_remove_file(android_touch_kobj, &dev_attr_glove_setting.attr);
		sysfs_remove_group(android_touch_kobj, &attr_group);
		kobject_del(android_touch_kobj);
		dev_info(rmi4_data->pdev->dev.parent,
				"%s: sysfs attributes are removed\n",
				__func__);
	}

	return 0;
}
#endif

#ifdef USE_DATA_SERVER
static ssize_t synaptics_rmi4_synad_pid_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int input;

	if (sscanf(buf, "%u", &input) != 1)
		return -EINVAL;

	synad_pid = input;

	if (synad_pid) {
		synad_task = pid_task(find_vpid(synad_pid), PIDTYPE_PID);
		if (!synad_task)
			return -EINVAL;
	}

	return count;
}
#endif

#if defined(CONFIG_SECURE_TOUCH)
static int synaptics_i2c_change_pipe_owner(
        struct synaptics_rmi4_data *rmi4_data, enum subsystem subsystem)
{
        /*scm call descriptor */
        struct scm_desc desc;
        struct i2c_client *i2c = to_i2c_client(rmi4_data->pdev->dev.parent);
        int ret = 0;

        /* number of arguments */
        desc.arginfo = SCM_ARGS(2);
        /* BLSPID (1-12) */
        desc.args[0] = i2c->adapter->nr - 1;
        /* Owner if TZ or APSS */
        desc.args[1] = subsystem;
        ret = scm_call2(SCM_SIP_FNID(SCM_SVC_TZ, TZ_BLSP_MODIFY_OWNERSHIP_ID),
                        &desc);
	if (ret)
                return ret;

        return desc.ret[0];
}

static void synaptics_secure_touch_init(struct synaptics_rmi4_data *data)
{
        data->st_initialized = 0;
        init_completion(&data->st_powerdown);
        init_completion(&data->st_irq_processed);
        data->st_initialized = 1;
}
static void synaptics_secure_touch_notify(struct synaptics_rmi4_data *rmi4_data)
{
        sysfs_notify(&rmi4_data->input_dev->dev.kobj, NULL, "secure_touch");
}
static irqreturn_t synaptics_filter_interrupt(
	struct synaptics_rmi4_data *rmi4_data)
{
        if (atomic_read(&rmi4_data->st_enabled)) {
                if (atomic_cmpxchg(&rmi4_data->st_pending_irqs, 0, 1) == 0) {
                        reinit_completion(&rmi4_data->st_irq_processed);
                        synaptics_secure_touch_notify(rmi4_data);
                        wait_for_completion_interruptible(
                                &rmi4_data->st_irq_processed);
                }
                return IRQ_HANDLED;
        }
        return IRQ_NONE;
}
static void synaptics_secure_touch_stop(
        struct synaptics_rmi4_data *rmi4_data,
        int blocking)
{
        if (atomic_read(&rmi4_data->st_enabled)) {
                atomic_set(&rmi4_data->st_pending_irqs, -1);
                synaptics_secure_touch_notify(rmi4_data);
                if (blocking)
                        wait_for_completion_interruptible(
                                &rmi4_data->st_powerdown);
        }
}
#else
static void synaptics_secure_touch_init(struct synaptics_rmi4_data *rmi4_data)
{
}
static irqreturn_t synaptics_filter_interrupt(
	struct synaptics_rmi4_data *rmi4_data)
{
        return IRQ_NONE;
}
static void synaptics_secure_touch_stop(
        struct synaptics_rmi4_data *rmi4_data,
        int blocking)
{
}
#endif

static ssize_t synaptics_rmi4_virtual_key_map_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	int ii;
	int cnt;
	int count = 0;

	for (ii = 0; ii < vir_button_map->nbuttons; ii++) {
		cnt = snprintf(buf, PAGE_SIZE - count, "0x01:%d:%d:%d:%d:%d\n",
				vir_button_map->map[ii * 5 + 0],
				vir_button_map->map[ii * 5 + 1],
				vir_button_map->map[ii * 5 + 2],
				vir_button_map->map[ii * 5 + 3],
				vir_button_map->map[ii * 5 + 4]);
		buf += cnt;
		count += cnt;
	}

	return count;
}

static void synaptics_rmi4_f11_wg(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	int retval;
	unsigned char reporting_control;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_device_info *rmi;

	rmi = &(rmi4_data->rmi4_mod_info);

	list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
		if (fhandler->fn_number == SYNAPTICS_RMI4_F11)
			break;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.ctrl_base,
			&reporting_control,
			sizeof(reporting_control));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to change reporting mode\n",
				__func__);
		return;
	}

	reporting_control = (reporting_control & ~MASK_3BIT);
	if (enable)
		reporting_control |= F11_WAKEUP_GESTURE_MODE;
	else
		reporting_control |= F11_CONTINUOUS_MODE;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			fhandler->full_addr.ctrl_base,
			&reporting_control,
			sizeof(reporting_control));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to change reporting mode\n",
				__func__);
		return;
	}

	return;
}

static void synaptics_rmi4_f12_wg(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	int retval;
	unsigned char offset;
	unsigned char reporting_control[3];
	struct synaptics_rmi4_f12_extra_data *extra_data;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_device_info *rmi;

	rmi = &(rmi4_data->rmi4_mod_info);

	list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
		if (fhandler->fn_number == SYNAPTICS_RMI4_F12)
			break;
	}

	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;
	offset = extra_data->ctrl20_offset;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.ctrl_base + offset,
			reporting_control,
			sizeof(reporting_control));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to change reporting mode\n",
				__func__);
		return;
	}

	if (enable)
		reporting_control[2] = F12_WAKEUP_GESTURE_MODE;
	else
		reporting_control[2] = F12_CONTINUOUS_MODE;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			fhandler->full_addr.ctrl_base + offset,
			reporting_control,
			sizeof(reporting_control));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to change reporting mode\n",
				__func__);
		return;
	}

	return;
}

static void synaptics_rmi4_wakeup_gesture(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	if (rmi4_data->f11_wakeup_gesture)
		synaptics_rmi4_f11_wg(rmi4_data, enable);
	else if (rmi4_data->f12_wakeup_gesture)
		synaptics_rmi4_f12_wg(rmi4_data, enable);

	return;
}

#ifdef HTC_FEATURE
static void report_touch_detect(struct synaptics_rmi4_data *rmi4_data)
{
	sysfs_notify(android_touch_kobj, NULL, "touch_detect");
}

static int synaptics_rmi4_get_noise_state(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	uint8_t data[2] = {0};
	struct synaptics_rmi4_noise_state noise_state;

	retval = synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f54_data_base_addr + rmi4_data->f54_im_offset,
					data, sizeof(data));
	if (retval < 0)
		return retval;

	noise_state.im = (data[1] << 8) | data[0];

	retval = synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f54_data_base_addr + rmi4_data->f54_ns_offset,
					data, sizeof(data[0]));
	if (retval < 0)
		return retval;

	noise_state.ns = data[0];

	retval = synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f54_data_base_addr + rmi4_data->f54_cidim_offset,
					data, sizeof(data));
	if (retval < 0)
		return retval;

	noise_state.cidim = (data[1] << 8) | data[0];

	retval = synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f54_data_base_addr + rmi4_data->f54_freq_offset,
					data, sizeof(data[0]));
	if (retval < 0)
		return retval;

	noise_state.freq = (data[0] & FREQ_MASK);

	noise_state.im_m = (noise_state.im > rmi4_data->noise_state.im_m) ?
			noise_state.im : rmi4_data->noise_state.im_m;
	noise_state.cidim_m = (noise_state.cidim > rmi4_data->noise_state.cidim_m) ?
			noise_state.cidim : rmi4_data->noise_state.cidim_m;

	if ((noise_state.freq != rmi4_data->noise_state.freq) ||
		(noise_state.ns != rmi4_data->noise_state.ns)) {
		dev_info(rmi4_data->pdev->dev.parent,
				"[NS]: IM:%d(M-%d), CIDIM:%d(M-%d), Freq:%d, NS:%d\n",
				noise_state.im,
				noise_state.im_m,
				noise_state.cidim,
				noise_state.cidim_m,
				noise_state.freq,
				noise_state.ns);

		noise_state.im_m = 0;
		noise_state.cidim_m = 0;
	}

	memcpy(&rmi4_data->noise_state, &noise_state, sizeof(noise_state));

	return 0;
}

const char* synaptics_rmi4_finger_dnup_state(enum finger_dnup state)
{
	switch (state) {
		case REPORT_FINGER_UP:
			return "Up  ";
			break;
		case REPORT_FINGER_DOWN:
			return "Down";
			break;
		case REPORT_FINGER_MOVE:
			return "Move";
			break;
	}
	return "Unsupport state";
}
#endif

static int synaptics_rmi4_f11_abs_report(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned char touch_count = 0; /* number of touch points */
	unsigned char reg_index;
	unsigned char finger;
	unsigned char fingers_supported;
	unsigned char num_of_finger_status_regs;
	unsigned char finger_shift;
	unsigned char finger_status;
	unsigned char finger_status_reg[3];
	unsigned char detected_gestures;
	unsigned short data_addr;
	unsigned short data_offset;
	int x;
	int y;
	int wx;
	int wy;
	int temp;
	struct synaptics_rmi4_f11_data_1_5 data;
	struct synaptics_rmi4_f11_extra_data *extra_data;

	/*
	 * The number of finger status registers is determined by the
	 * maximum number of fingers supported - 2 bits per finger. So
	 * the number of finger status registers to read is:
	 * register_count = ceil(max_num_of_fingers / 4)
	 */
	fingers_supported = fhandler->num_of_data_points;
	num_of_finger_status_regs = (fingers_supported + 3) / 4;
	data_addr = fhandler->full_addr.data_base;

	extra_data = (struct synaptics_rmi4_f11_extra_data *)fhandler->extra;

	if (rmi4_data->suspend && rmi4_data->enable_wakeup_gesture) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data38_offset,
				&detected_gestures,
				sizeof(detected_gestures));
		if (retval < 0)
			return 0;

		if (detected_gestures) {
			input_report_key(rmi4_data->input_dev, KEY_WAKEUP, 1);
			input_sync(rmi4_data->input_dev);
			input_report_key(rmi4_data->input_dev, KEY_WAKEUP, 0);
			input_sync(rmi4_data->input_dev);
			rmi4_data->suspend = false;
		}
		synaptics_rmi4_wakeup_gesture(rmi4_data, false);
		return 0;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			data_addr,
			finger_status_reg,
			num_of_finger_status_regs);
	if (retval < 0)
		return 0;

	mutex_lock(&(rmi4_data->rmi4_report_mutex));

	for (finger = 0; finger < fingers_supported; finger++) {
		reg_index = finger / 4;
		finger_shift = (finger % 4) * 2;
		finger_status = (finger_status_reg[reg_index] >> finger_shift)
				& MASK_2BIT;

		/*
		 * Each 2-bit finger status field represents the following:
		 * 00 = finger not present
		 * 01 = finger present and data accurate
		 * 10 = finger present but data may be inaccurate
		 * 11 = reserved
		 */
#ifdef TYPE_B_PROTOCOL
		input_mt_slot(rmi4_data->input_dev, finger);
		input_mt_report_slot_state(rmi4_data->input_dev,
				MT_TOOL_FINGER, finger_status);
#endif

		if (finger_status) {
			data_offset = data_addr +
					num_of_finger_status_regs +
					(finger * sizeof(data.data));
			retval = synaptics_rmi4_reg_read(rmi4_data,
					data_offset,
					data.data,
					sizeof(data.data));
			if (retval < 0) {
				touch_count = 0;
				goto exit;
			}

			x = (data.x_position_11_4 << 4) | data.x_position_3_0;
			y = (data.y_position_11_4 << 4) | data.y_position_3_0;
			wx = data.wx;
			wy = data.wy;

			if (rmi4_data->hw_if->board_data->swap_axes) {
				temp = x;
				x = y;
				y = temp;
				temp = wx;
				wx = wy;
				wy = temp;
			}

			if (rmi4_data->hw_if->board_data->x_flip)
				x = rmi4_data->sensor_max_x - x;
			if (rmi4_data->hw_if->board_data->y_flip)
				y = rmi4_data->sensor_max_y - y;

			input_report_key(rmi4_data->input_dev,
					BTN_TOUCH, 1);
			input_report_key(rmi4_data->input_dev,
					BTN_TOOL_FINGER, 1);
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_POSITION_X, x);
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_POSITION_Y, y);
#ifdef REPORT_2D_W
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_TOUCH_MAJOR, max(wx, wy));
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_TOUCH_MINOR, min(wx, wy));
#endif
#ifndef TYPE_B_PROTOCOL
			input_mt_sync(rmi4_data->input_dev);
#endif

			dev_dbg(rmi4_data->pdev->dev.parent,
					"%s: Finger %d: status = 0x%02x, x = %d, y = %d, wx = %d, wy = %d\n",
					__func__, finger,
					finger_status,
					x, y, wx, wy);

			touch_count++;
		}
	}

	if (touch_count == 0) {
		input_report_key(rmi4_data->input_dev,
				BTN_TOUCH, 0);
		input_report_key(rmi4_data->input_dev,
				BTN_TOOL_FINGER, 0);
#ifndef TYPE_B_PROTOCOL
		input_mt_sync(rmi4_data->input_dev);
#endif
	}

	input_sync(rmi4_data->input_dev);

exit:
	mutex_unlock(&(rmi4_data->rmi4_report_mutex));

	return touch_count;
}

static int synaptics_rmi4_f12_abs_report(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned char touch_count = 0; /* number of touch points */
	unsigned char index;
	unsigned char finger;
	unsigned char fingers_to_process;
	unsigned char finger_status;
	unsigned char size_of_2d_data;
	unsigned char gesture_type;
	unsigned short data_addr;
	int x;
	int y;
	int wx;
	int wy;
	int temp;
#if defined(REPORT_2D_PRESSURE) || defined(F51_DISCRETE_FORCE)
	int pressure;
#endif
#ifdef REPORT_2D_PRESSURE
	unsigned char f_fingers;
	unsigned char f_lsb;
	unsigned char f_msb;
	unsigned char *f_data;
#endif
#ifdef F51_DISCRETE_FORCE
	unsigned char force_level;
#endif
	struct synaptics_rmi4_f12_extra_data *extra_data;
	struct synaptics_rmi4_f12_finger_data *data;
	struct synaptics_rmi4_f12_finger_data *finger_data;
	static unsigned char finger_presence;
	static unsigned char stylus_presence;
#ifdef F12_DATA_15_WORKAROUND
	static unsigned char objects_already_present;
#endif
#ifdef HTC_FEATURE
	bool touch_detect = false;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: ++\n", __func__);
#endif

	fingers_to_process = fhandler->num_of_data_points;
	data_addr = fhandler->full_addr.data_base;
	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;
	size_of_2d_data = sizeof(struct synaptics_rmi4_f12_finger_data);

#ifdef HTC_FEATURE
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_TIME)
		getnstimeofday(&rmi4_data->tp_report_read_start_time);
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: REPORT_I2C_READ START\n", __func__);
#endif

	if (rmi4_data->suspend && rmi4_data->enable_wakeup_gesture) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data4_offset,
				rmi4_data->gesture_detection,
				sizeof(rmi4_data->gesture_detection));
		if (retval < 0)
			return 0;

		gesture_type = rmi4_data->gesture_detection[0];

		if (gesture_type && gesture_type != F12_UDG_DETECT) {
			input_report_key(rmi4_data->input_dev, KEY_WAKEUP, 1);
			input_sync(rmi4_data->input_dev);
			input_report_key(rmi4_data->input_dev, KEY_WAKEUP, 0);
			input_sync(rmi4_data->input_dev);
			synaptics_rmi4_wakeup_gesture(rmi4_data, false);
			rmi4_data->suspend = false;
		}

		return 0;
	}

	/* Determine the total number of fingers to process */
	if (extra_data->data15_size) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data15_offset,
				extra_data->data15_data,
				extra_data->data15_size);
		if (retval < 0)
			return 0;

		/* Start checking from the highest bit */
		index = extra_data->data15_size - 1; /* Highest byte */
		finger = (fingers_to_process - 1) % 8; /* Highest bit */
		do {
			if (extra_data->data15_data[index] & (1 << finger))
				break;

			if (finger) {
				finger--;
			} else if (index > 0) {
				index--; /* Move to the next lower byte */
				finger = 7;
			}

			fingers_to_process--;
		} while (fingers_to_process);

		dev_dbg(rmi4_data->pdev->dev.parent,
			"%s: Number of fingers to process = %d\n",
			__func__, fingers_to_process);
	}

#ifdef F12_DATA_15_WORKAROUND
	fingers_to_process = max(fingers_to_process, objects_already_present);
#endif

	if (!fingers_to_process) {
		synaptics_rmi4_free_fingers(rmi4_data);
		finger_presence = 0;
		stylus_presence = 0;
		return 0;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			data_addr + extra_data->data1_offset,
			(unsigned char *)fhandler->data,
			fingers_to_process * size_of_2d_data);
	if (retval < 0)
		return 0;

	data = (struct synaptics_rmi4_f12_finger_data *)fhandler->data;

#ifdef REPORT_2D_PRESSURE
	if (rmi4_data->report_pressure) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				data_addr + extra_data->data29_offset,
				extra_data->data29_data,
				extra_data->data29_size);
		if (retval < 0)
			return 0;
	}
#endif

#ifdef HTC_FEATURE
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_TIME)
		getnstimeofday(&rmi4_data->tp_report_read_done_time);
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: REPORT_I2C_READ DONE\n", __func__);
#endif

	mutex_lock(&(rmi4_data->rmi4_report_mutex));

	for (finger = 0; finger < fingers_to_process; finger++) {
#ifdef HTC_FEATURE
		if (rmi4_data->debug_mask & (TOUCH_DOWN_UP_LOG | TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME))
			rmi4_data->report_points[finger].finger_ind = 0;
#endif
		finger_data = data + finger;
		finger_status = finger_data->object_type_and_status;

#ifdef F12_DATA_15_WORKAROUND
		objects_already_present = finger + 1;
#endif

		x = (finger_data->x_msb << 8) | (finger_data->x_lsb);
		y = (finger_data->y_msb << 8) | (finger_data->y_lsb);
#ifdef REPORT_2D_W
		wx = finger_data->wx;
		wy = finger_data->wy;
#endif

		if (rmi4_data->hw_if->board_data->swap_axes) {
			temp = x;
			x = y;
			y = temp;
			temp = wx;
			wx = wy;
			wy = temp;
		}

		if (rmi4_data->hw_if->board_data->x_flip)
			x = rmi4_data->sensor_max_x - x;
		if (rmi4_data->hw_if->board_data->y_flip)
			y = rmi4_data->sensor_max_y - y;

		switch (finger_status) {
		case F12_FINGER_STATUS:
		case F12_GLOVED_FINGER_STATUS:
			/* Stylus has priority over fingers */
			if (stylus_presence)
				break;
#ifdef TYPE_B_PROTOCOL
			input_mt_slot(rmi4_data->input_dev, finger);
			input_mt_report_slot_state(rmi4_data->input_dev,
					MT_TOOL_FINGER, 1);
#endif

			input_report_key(rmi4_data->input_dev,
					BTN_TOUCH, 1);
			input_report_key(rmi4_data->input_dev,
					BTN_TOOL_FINGER, 1);
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_POSITION_X, x);
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_POSITION_Y, y);
#ifdef REPORT_2D_W
			if (rmi4_data->wedge_sensor) {
				input_report_abs(rmi4_data->input_dev,
						ABS_MT_TOUCH_MAJOR, wx);
				input_report_abs(rmi4_data->input_dev,
						ABS_MT_TOUCH_MINOR, wx);
			} else {
				input_report_abs(rmi4_data->input_dev,
						ABS_MT_TOUCH_MAJOR,
						max(wx, wy));
				input_report_abs(rmi4_data->input_dev,
						ABS_MT_TOUCH_MINOR,
						min(wx, wy));
			}
#endif
#ifdef REPORT_2D_PRESSURE
			if (rmi4_data->report_pressure) {
				f_fingers = extra_data->data29_size / 2;
				f_data = extra_data->data29_data;
				if (finger + 1 > f_fingers) {
					pressure = 1;
				} else {
					f_lsb = finger * 2;
					f_msb = finger * 2 + 1;
					pressure = (int)f_data[f_lsb] << 0 |
							(int)f_data[f_msb] << 8;
				}
				pressure = pressure > 0 ? pressure : 1;
				if (pressure > rmi4_data->force_max)
					pressure = rmi4_data->force_max;
				input_report_abs(rmi4_data->input_dev,
						ABS_MT_PRESSURE, pressure);
			}
#elif defined(F51_DISCRETE_FORCE)
			if (finger == 0) {
				retval = synaptics_rmi4_reg_read(rmi4_data,
						FORCE_LEVEL_ADDR,
						&force_level,
						sizeof(force_level));
				if (retval < 0)
					return 0;
				pressure = force_level > 0 ? force_level : 1;
			} else {
				pressure = 1;
			}
			input_report_abs(rmi4_data->input_dev,
					ABS_MT_PRESSURE, pressure);
#endif
#ifndef TYPE_B_PROTOCOL
			input_mt_sync(rmi4_data->input_dev);
#endif

#ifdef HTC_FEATURE
			if (rmi4_data->factor_width && rmi4_data->factor_height) {
				if (rmi4_data->debug_mask & SHOW_INT_I2C_BUF) {
					dev_info(rmi4_data->pdev->dev.parent,
							"%s: Finger %d: status = %#02x, x = %d, y = %d, wx = %d, wy = %d\n",
							__func__, finger,
							finger_status,
							(x * rmi4_data->factor_width) >> RESOL_SHIFT_BITS,
							(y * rmi4_data->factor_height) >> RESOL_SHIFT_BITS,
							wx, wy);
				}

				if (rmi4_data->debug_mask & TOUCH_DOWN_UP_LOG) {
					rmi4_data->report_points[finger].x = (x * rmi4_data->factor_width) >> RESOL_SHIFT_BITS;
					rmi4_data->report_points[finger].y = (y * rmi4_data->factor_height) >> RESOL_SHIFT_BITS;

					if (rmi4_data->report_points[finger].state != 1) {
						rmi4_data->report_points[finger].finger_ind = finger + 1;
						rmi4_data->report_points[finger].dnup = REPORT_FINGER_DOWN;
						rmi4_data->report_points[finger].wx = wx;
						rmi4_data->report_points[finger].wy = wy;
						rmi4_data->report_points[finger].state = 1;
					}
				} else if (rmi4_data->debug_mask & (TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME)) {
					rmi4_data->report_points[finger].x = (x * rmi4_data->factor_width) >> RESOL_SHIFT_BITS;
					rmi4_data->report_points[finger].y = (y * rmi4_data->factor_height) >> RESOL_SHIFT_BITS;

					if (rmi4_data->report_points[finger].state != 1) {
						rmi4_data->report_points[finger].finger_ind = finger + 1;
						rmi4_data->report_points[finger].dnup = REPORT_FINGER_DOWN;
						rmi4_data->report_points[finger].wx = wx;
						rmi4_data->report_points[finger].wy = wy;
						rmi4_data->report_points[finger].state = 1;
					} else {
						rmi4_data->report_points[finger].finger_ind = finger + 1;
						rmi4_data->report_points[finger].dnup = REPORT_FINGER_MOVE;
						rmi4_data->report_points[finger].wx = wx;
						rmi4_data->report_points[finger].wy = wy;
					}
				}
			}

			if (bdata->support_eg_blk && !touch_detect) {
				temp = x * 1000 / rmi4_data->sensor_max_x;
				if (temp > bdata->eg_blk_permille && temp < (1000 - bdata->eg_blk_permille)
						&& (wx >= bdata->eg_blk_min_width || wy >= bdata->eg_blk_min_width)) {
					if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG) {
						pr_info("per: %d (x: %d, max: %d), wx: %d, wy: %d\n", temp, x, rmi4_data->sensor_max_x, wx, wy);
					}
					touch_detect = true;
				}
			}
#else
			dev_dbg(rmi4_data->pdev->dev.parent,
					"%s: Finger %d: status = 0x%02x, x = %d, y = %d, wx = %d, wy = %d\n",
					__func__, finger,
					finger_status,
					x, y, wx, wy);
#endif

			finger_presence = 1;
			touch_count++;
			break;
		case F12_PALM_STATUS:
			dev_dbg(rmi4_data->pdev->dev.parent,
					"%s: Finger %d: x = %d, y = %d, wx = %d, wy = %d\n",
					__func__, finger,
					x, y, wx, wy);
			break;
		case F12_STYLUS_STATUS:
		case F12_ERASER_STATUS:
			if (!rmi4_data->stylus_enable)
				break;
			/* Stylus has priority over fingers */
			if (finger_presence) {
				mutex_unlock(&(rmi4_data->rmi4_report_mutex));
				synaptics_rmi4_free_fingers(rmi4_data);
				mutex_lock(&(rmi4_data->rmi4_report_mutex));
				finger_presence = 0;
			}
			if (stylus_presence) {/* Allow one stylus at a timee */
				if (finger + 1 != stylus_presence)
					break;
			}
			input_report_key(rmi4_data->stylus_dev,
					BTN_TOUCH, 1);
			if (finger_status == F12_STYLUS_STATUS) {
				input_report_key(rmi4_data->stylus_dev,
						BTN_TOOL_PEN, 1);
			} else {
				input_report_key(rmi4_data->stylus_dev,
						BTN_TOOL_RUBBER, 1);
			}
			input_report_abs(rmi4_data->stylus_dev,
					ABS_X, x);
			input_report_abs(rmi4_data->stylus_dev,
					ABS_Y, y);
			input_sync(rmi4_data->stylus_dev);

			stylus_presence = finger + 1;
			touch_count++;
			break;
		default:
#ifdef TYPE_B_PROTOCOL
			input_mt_slot(rmi4_data->input_dev, finger);
			input_mt_report_slot_state(rmi4_data->input_dev,
					MT_TOOL_FINGER, 0);
#endif

#ifdef HTC_FEATURE
			if (rmi4_data->debug_mask & (TOUCH_DOWN_UP_LOG | TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME)) {
				if (rmi4_data->report_points[finger].state != 0) {
					rmi4_data->report_points[finger].finger_ind = finger + 1;
					rmi4_data->report_points[finger].dnup = REPORT_FINGER_UP;
					rmi4_data->report_points[finger].wx = wx;
					rmi4_data->report_points[finger].wy = wy;
					rmi4_data->report_points[finger].state = 0;
				}
			}
#endif

			break;
		}
	}

	if (touch_count == 0) {
		finger_presence = 0;
#ifdef F12_DATA_15_WORKAROUND
		objects_already_present = 0;
#endif
		input_report_key(rmi4_data->input_dev,
				BTN_TOUCH, 0);
		input_report_key(rmi4_data->input_dev,
				BTN_TOOL_FINGER, 0);
#ifndef TYPE_B_PROTOCOL
		input_mt_sync(rmi4_data->input_dev);
#endif

		if (rmi4_data->stylus_enable) {
			stylus_presence = 0;
			input_report_key(rmi4_data->stylus_dev,
					BTN_TOUCH, 0);
			input_report_key(rmi4_data->stylus_dev,
					BTN_TOOL_PEN, 0);
			if (rmi4_data->eraser_enable) {
				input_report_key(rmi4_data->stylus_dev,
						BTN_TOOL_RUBBER, 0);
			}
			input_sync(rmi4_data->stylus_dev);
		}

#ifdef HTC_FEATURE
		if (bdata->support_eg_blk) {
			touch_detect = false;
		}

		if (rmi4_data->debug_mask & (TOUCH_DOWN_UP_LOG | TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME)) {
			for (finger = 0; finger < fingers_to_process; finger++) {
				if (rmi4_data->report_points[finger].state != 0) {
					rmi4_data->report_points[finger].finger_ind = finger + 1;
					rmi4_data->report_points[finger].dnup = REPORT_FINGER_UP;
					rmi4_data->report_points[finger].wx = wx;
					rmi4_data->report_points[finger].wy = wy;
					rmi4_data->report_points[finger].state = 0;
				}
			}
		}
#endif
	}

	input_sync(rmi4_data->input_dev);

#ifdef HTC_FEATURE
	if (bdata->support_eg_blk && rmi4_data->touch_detect != touch_detect) {
		rmi4_data->touch_detect = touch_detect;
		if (rmi4_data->debug_mask & TOUCH_DOWN_UP_LOG) {
			pr_info("set touch_detect %s - permi:%d(%d/%d), wx:%d, wy:%d\n",
					rmi4_data->touch_detect ? "enabled" : "disabled",
					x * 1000 / rmi4_data->sensor_max_x, x, rmi4_data->sensor_max_x, wx, wy);
		}
		report_touch_detect(rmi4_data);
	}

	if (rmi4_data->debug_mask & (TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME)) {
		getnstimeofday(&rmi4_data->tp_sync_time);
//		if (rmi4_data->report_points[0].y > 0)	//systrace
//			trace_clock_set_rate("tp_report", rmi4_data->report_points[0].y, -4);
	}
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: input_sync done\n", __func__);

	synaptics_rmi4_get_noise_state(rmi4_data);
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: get_noise_state done\n", __func__);

	if (rmi4_data->debug_mask & TOUCH_DOWN_UP_LOG) {
		if (rmi4_data->factor_width && rmi4_data->factor_height) {
			for (finger = 0; finger < fingers_to_process; finger++) {
				if (rmi4_data->report_points[finger].finger_ind != 0) {
					dev_info(rmi4_data->pdev->dev.parent,
							"Screen:%c[%02d]:%s, X=%d, Y=%d, Wx=%d, Wy=%d, IM:%d(M-%d), CIDIM:%d(M-%d), Freq:%d, NS:%d\n",
							(finger_status == F12_GLOVED_FINGER_STATUS) ? 'G' : 'F',
							rmi4_data->report_points[finger].finger_ind,
							(rmi4_data->report_points[finger].dnup)?"Down":"Up",
							rmi4_data->report_points[finger].x,
							rmi4_data->report_points[finger].y,
							rmi4_data->report_points[finger].wx,
							rmi4_data->report_points[finger].wy,
							rmi4_data->noise_state.im,
							rmi4_data->noise_state.im_m,
							rmi4_data->noise_state.cidim,
							rmi4_data->noise_state.cidim_m,
							rmi4_data->noise_state.freq,
							rmi4_data->noise_state.ns);
				}
			}
		}
	} else if (rmi4_data->debug_mask & TOUCH_KPI_LOG) {
		if (rmi4_data->factor_width && rmi4_data->factor_height) {
			for (finger = 0; finger < fingers_to_process; finger++) {
				if (rmi4_data->report_points[finger].finger_ind != 0) {
					pr_info("Screen:%c[%02d]:%s, X=%4d, Y=%4d, Wx=%2d, Wy=%2d, time:  %ld.%06ld  to  %ld.%06ld\n",
							(finger_status == F12_GLOVED_FINGER_STATUS) ? 'G' : 'F',
							rmi4_data->report_points[finger].finger_ind,
							synaptics_rmi4_finger_dnup_state(rmi4_data->report_points[finger].dnup),
							rmi4_data->report_points[finger].x,
							rmi4_data->report_points[finger].y,
							rmi4_data->report_points[finger].wx,
							rmi4_data->report_points[finger].wy,
							rmi4_data->tp_handler_time.tv_sec % 1000,
							rmi4_data->tp_handler_time.tv_nsec / 1000,
							rmi4_data->tp_sync_time.tv_sec % 1000,
							rmi4_data->tp_sync_time.tv_nsec / 1000);
				}
			}
		}
	} else if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_TIME) {
		if (rmi4_data->factor_width && rmi4_data->factor_height) {
			for (finger = 0; finger < fingers_to_process; finger++) {
				if (rmi4_data->report_points[finger].finger_ind != 0) {
					pr_info("Screen:%c[%02d]:%s, X=%4d, Y=%4d, Wx=%2d, Wy=%2d, time:  %4ld  BUS:  %4ld\n",
							(finger_status == F12_GLOVED_FINGER_STATUS) ? 'G' : 'F',
							rmi4_data->report_points[finger].finger_ind,
							synaptics_rmi4_finger_dnup_state(rmi4_data->report_points[finger].dnup),
							rmi4_data->report_points[finger].x,
							rmi4_data->report_points[finger].y,
							rmi4_data->report_points[finger].wx,
							rmi4_data->report_points[finger].wy,
							((rmi4_data->tp_sync_time.tv_sec * 1000000000 + rmi4_data->tp_sync_time.tv_nsec)
									- (rmi4_data->tp_handler_time.tv_sec * 1000000000 + rmi4_data->tp_handler_time.tv_nsec)) / 1000,
							((rmi4_data->tp_read_done_time.tv_sec * 1000000000 + rmi4_data->tp_read_done_time.tv_nsec)
									- (rmi4_data->tp_read_start_time.tv_sec * 1000000000 + rmi4_data->tp_read_start_time.tv_nsec)
									+ (rmi4_data->tp_report_read_done_time.tv_sec * 1000000000 + rmi4_data->tp_report_read_done_time.tv_nsec)
									- (rmi4_data->tp_report_read_start_time.tv_sec * 1000000000 + rmi4_data->tp_report_read_start_time.tv_nsec)) / 1000);
				}
			}
		}
	}
#endif

	mutex_unlock(&(rmi4_data->rmi4_report_mutex));

	return touch_count;
}

static void synaptics_rmi4_f1a_report(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned char touch_count = 0;
	unsigned char button;
	unsigned char index;
	unsigned char shift;
	unsigned char status;
	unsigned char *data;
	unsigned short data_addr = fhandler->full_addr.data_base;
	struct synaptics_rmi4_f1a_handle *f1a = fhandler->data;
	static unsigned char do_once = 1;
	static bool current_status[MAX_NUMBER_OF_BUTTONS];
#ifdef NO_0D_WHILE_2D
	static bool before_2d_status[MAX_NUMBER_OF_BUTTONS];
	static bool while_2d_status[MAX_NUMBER_OF_BUTTONS];
#endif

	if (do_once) {
		memset(current_status, 0, sizeof(current_status));
#ifdef NO_0D_WHILE_2D
		memset(before_2d_status, 0, sizeof(before_2d_status));
		memset(while_2d_status, 0, sizeof(while_2d_status));
#endif
		do_once = 0;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			data_addr,
			f1a->button_data_buffer,
			f1a->button_bitmask_size);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read button data registers\n",
				__func__);
		return;
	}

	data = f1a->button_data_buffer;

	mutex_lock(&(rmi4_data->rmi4_report_mutex));

	for (button = 0; button < f1a->valid_button_count; button++) {
		index = button / 8;
		shift = button % 8;
		status = ((data[index] >> shift) & MASK_1BIT);

		if (current_status[button] == status)
			continue;
		else
			current_status[button] = status;

		dev_dbg(rmi4_data->pdev->dev.parent,
				"%s: Button %d (code %d) ->%d\n",
				__func__, button,
				f1a->button_map[button],
				status);
#ifdef NO_0D_WHILE_2D
		if (rmi4_data->fingers_on_2d == false) {
			if (status == 1) {
				before_2d_status[button] = 1;
			} else {
				if (while_2d_status[button] == 1) {
					while_2d_status[button] = 0;
					continue;
				} else {
					before_2d_status[button] = 0;
				}
			}
			touch_count++;
			input_report_key(rmi4_data->input_dev,
					f1a->button_map[button],
					status);
		} else {
			if (before_2d_status[button] == 1) {
				before_2d_status[button] = 0;
				touch_count++;
				input_report_key(rmi4_data->input_dev,
						f1a->button_map[button],
						status);
			} else {
				if (status == 1)
					while_2d_status[button] = 1;
				else
					while_2d_status[button] = 0;
			}
		}
#else
		touch_count++;
		input_report_key(rmi4_data->input_dev,
				f1a->button_map[button],
				status);
#endif
	}

	if (touch_count)
		input_sync(rmi4_data->input_dev);

	mutex_unlock(&(rmi4_data->rmi4_report_mutex));

	return;
}

#ifdef HTC_FEATURE
static void synaptics_rmi4_f54_report(struct synaptics_rmi4_data *rmi4_data)
{
	int ret, size;
	unsigned char *read_data;
	uint8_t data[2] = {0};

	ret = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f54_data_base_addr + REPORT_INDEX_OFFSET,
			data,
			sizeof(data));

	if (ret < 0) {
		dev_err(rmi4_data->pdev->dev.parent," %s: Write error\n", __func__);
	} else {
		if (rmi4_data->diag_command == F54_16BIT_IMAGE || rmi4_data->diag_command == F54_RAW_16BIT_IMAGE) {
			size = rmi4_data->num_of_tx * rmi4_data->num_of_rx * 2;
			read_data = (unsigned char *)rmi4_data->report_data;
		} else {
			size = (rmi4_data->num_of_tx + rmi4_data->num_of_rx) * 4;
			read_data = (unsigned char *)rmi4_data->report_data_32;
		}

		if (read_data != NULL && size != 0) {
			ret = synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f54_data_base_addr + REPORT_DATA_OFFSET,
					read_data,
					size);

			if (ret < 0) {
				memset(read_data, 0x0, size);
				dev_err(rmi4_data->pdev->dev.parent," %s: Read error\n", __func__);
			}
		}
	}

	atomic_set(&rmi4_data->data_ready, 1);
	wake_up(&syn_data_ready_wq);
}
#endif

static void synaptics_rmi4_report_touch(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler)
{
	unsigned char touch_count_2d;

#ifdef HTC_FEATURE
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: Function %02x reporting\n", __func__, fhandler->fn_number);
#endif

	dev_dbg(rmi4_data->pdev->dev.parent,
			"%s: Function %02x reporting\n",
			__func__, fhandler->fn_number);

	switch (fhandler->fn_number) {
	case SYNAPTICS_RMI4_F11:
		touch_count_2d = synaptics_rmi4_f11_abs_report(rmi4_data,
				fhandler);

		if (touch_count_2d)
			rmi4_data->fingers_on_2d = true;
		else
			rmi4_data->fingers_on_2d = false;
		break;
	case SYNAPTICS_RMI4_F12:
		touch_count_2d = synaptics_rmi4_f12_abs_report(rmi4_data,
				fhandler);

		if (touch_count_2d)
			rmi4_data->fingers_on_2d = true;
		else
			rmi4_data->fingers_on_2d = false;
		break;
	case SYNAPTICS_RMI4_F1A:
		synaptics_rmi4_f1a_report(rmi4_data, fhandler);
		break;
#ifdef USE_DATA_SERVER
	case SYNAPTICS_RMI4_F21:
		if (synad_pid)
			send_sig_info(SIGIO, &interrupt_signal, synad_task);
		break;
#endif
#ifdef HTC_FEATURE
	case SYNAPTICS_RMI4_F54:
		synaptics_rmi4_f54_report(rmi4_data);
		break;
#endif
	default:
		break;
	}

	return;
}

static void synaptics_rmi4_sensor_report(struct synaptics_rmi4_data *rmi4_data,
		bool report)
{
	int retval;
	unsigned char data[MAX_INTR_REGISTERS + 1];
	unsigned char *intr = &data[1];
	bool was_in_bl_mode;
	struct synaptics_rmi4_f01_device_status status;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_device_info *rmi;

	rmi = &(rmi4_data->rmi4_mod_info);

	/*
	 * Get interrupt status information from F01 Data1 register to
	 * determine the source(s) that are flagging the interrupt.
	 */
#ifdef HTC_FEATURE
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_TIME)
		getnstimeofday(&rmi4_data->tp_read_start_time);
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: INT_I2C_READ START\n", __func__);
#endif
	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_data_base_addr,
			data,
			rmi4_data->num_of_intr_regs + 1);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read interrupt status\n",
				__func__);
		return;
	}

	status.data[0] = data[0];
	if (status.status_code == STATUS_CRC_IN_PROGRESS) {
		retval = synaptics_rmi4_check_status(rmi4_data,
				&was_in_bl_mode);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to check status\n",
					__func__);
			return;
		}
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f01_data_base_addr,
				status.data,
				sizeof(status.data));
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to read device status\n",
					__func__);
			return;
		}
	}
#ifdef HTC_FEATURE
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_TIME)
		getnstimeofday(&rmi4_data->tp_read_done_time);
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: INT_I2C_READ DONE\n", __func__);
#endif

#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
	if ((status.unconfigured && !status.flash_prog)
				|| (status.status_code == STATUS_ESD_DETECTED)) {
		pr_info("%s: ESD or spontaneous reset detected\n", __func__);
		if ((!rmi4_data->skip_esd || status.status_code == STATUS_ESD_DETECTED)
					&& set_lcm_rst) {
			pr_info("%s: set_lcm_rst\n", __func__);
			set_lcm_rst();
			return;
		} else if (!set_lcm_rst)
			pr_err("%s: not support set_lcm_rst\n", __func__);
#else
	if (status.unconfigured && !status.flash_prog) {
		pr_notice("%s: spontaneous reset detected\n", __func__);
#endif
		retval = synaptics_rmi4_reinit_device(rmi4_data);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to reinit device\n",
					__func__);
		}
	}

	if (!report)
		return;

	/*
	 * Traverse the function handler list and service the source(s)
	 * of the interrupt accordingly.
	 */
	if (!list_empty(&rmi->support_fn_list)) {
		list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
			if (fhandler->num_of_data_sources) {
				if (fhandler->intr_mask &
						intr[fhandler->intr_reg_num]) {
					synaptics_rmi4_report_touch(rmi4_data,
							fhandler);
				}
			}
		}
	}

	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link) {
			if (!exp_fhandler->insert &&
					!exp_fhandler->remove &&
					(exp_fhandler->exp_fn->attn != NULL))
				exp_fhandler->exp_fn->attn(rmi4_data, intr[0]);
		}
	}
	mutex_unlock(&exp_data.mutex);

	return;
}

static irqreturn_t synaptics_rmi4_irq(int irq, void *data)
{
	struct synaptics_rmi4_data *rmi4_data = data;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (gpio_get_value(bdata->irq_gpio) != bdata->irq_on_state)
		goto exit;

    if (IRQ_HANDLED == synaptics_filter_interrupt(data))
        return IRQ_HANDLED;

#ifdef HTC_FEATURE
	if (rmi4_data->debug_mask & (TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME))
		getnstimeofday(&rmi4_data->tp_handler_time);
	if (rmi4_data->debug_mask & TOUCH_BREAKDOWN_LOG)
		pr_info("[TP][KPI] %s: ++\n", __func__);
#endif

	synaptics_rmi4_sensor_report(rmi4_data, true);

exit:
	return IRQ_HANDLED;
}

static int synaptics_rmi4_int_enable(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	int retval = 0;
	unsigned char ii;
	unsigned char zero = 0x00;
	unsigned char *intr_mask;
	unsigned short intr_addr;

	intr_mask = rmi4_data->intr_mask;

	for (ii = 0; ii < rmi4_data->num_of_intr_regs; ii++) {
		if (intr_mask[ii] != 0x00) {
			intr_addr = rmi4_data->f01_ctrl_base_addr + 1 + ii;
			if (enable) {
				retval = synaptics_rmi4_reg_write(rmi4_data,
						intr_addr,
						&(intr_mask[ii]),
						sizeof(intr_mask[ii]));
				if (retval < 0)
					return retval;
			} else {
				retval = synaptics_rmi4_reg_write(rmi4_data,
						intr_addr,
						&zero,
						sizeof(zero));
				if (retval < 0)
					return retval;
			}
		}
	}

	return retval;
}

static int synaptics_rmi4_irq_enable(struct synaptics_rmi4_data *rmi4_data,
		bool enable, bool attn_only)
{
	int retval = 0;
	unsigned char data[MAX_INTR_REGISTERS];
#ifndef HTC_FEATURE		//NOT defined
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;
#endif

	if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
		pr_info("%s: %s\n", __func__, enable ? "enable" : "disable");
	mutex_lock(&(rmi4_data->rmi4_irq_enable_mutex));

	if (attn_only) {
		retval = synaptics_rmi4_int_enable(rmi4_data, enable);
		goto exit;
	}

	if (enable) {
		if (rmi4_data->irq_enabled) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Interrupt already enabled\n",
					__func__);
			goto exit;
		}

		retval = synaptics_rmi4_int_enable(rmi4_data, false);
		if (retval < 0)
			goto exit;

		/* Clear interrupts */
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f01_data_base_addr + 1,
				data,
				rmi4_data->num_of_intr_regs);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to read interrupt status\n",
					__func__);
			goto exit;
		}

#ifdef HTC_FEATURE
		enable_irq(rmi4_data->irq);
#else
		retval = request_threaded_irq(rmi4_data->irq, NULL,
				synaptics_rmi4_irq, bdata->irq_flags,
				PLATFORM_DRIVER_NAME, rmi4_data);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to create irq thread\n",
					__func__);
			goto exit;
		}
#endif

		retval = synaptics_rmi4_int_enable(rmi4_data, true);
		if (retval < 0)
			goto exit;

		rmi4_data->irq_enabled = true;
	} else {
		if (rmi4_data->irq_enabled) {
			disable_irq(rmi4_data->irq);
#ifndef HTC_FEATURE		//NOT defined
			free_irq(rmi4_data->irq, rmi4_data);
#endif
			rmi4_data->irq_enabled = false;
		}
	}

exit:
	mutex_unlock(&(rmi4_data->rmi4_irq_enable_mutex));

	return retval;
}

#if defined(CONFIG_SECURE_TOUCH)
static ssize_t synaptics_secure_touch_enable_show(struct device *dev,
                                struct device_attribute *attr, char *buf)
{
        struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);
        return scnprintf(
                buf,
                PAGE_SIZE,
                "%d",
                atomic_read(&rmi4_data->st_enabled));
}
/*
 * Accept only "0" and "1" valid values.
 * "0" will reset the st_enabled flag, then wake up the reading process and
 * the interrupt handler.
 * The bus driver is notified via pm_runtime that it is not required to stay
 * awake anymore.
 * It will also make sure the queue of events is emptied in the controller,
 * in case a touch happened in between the secure touch being disabled and
 * the local ISR being ungated.
 * "1" will set the st_enabled flag and clear the st_pending_irqs flag.
 * The bus driver is requested via pm_runtime to stay awake.
 */
static ssize_t synaptics_secure_touch_enable_store(struct device *dev,
                                    struct device_attribute *attr,
                                    const char *buf, size_t count)
{
        struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);
        unsigned long value;
        int err = 0;

        if (count > 2)
                return -EINVAL;

        err = kstrtoul(buf, 10, &value);
        if (err != 0)
                return err;

        if (!rmi4_data->st_initialized)
                return -EIO;

        err = count;

        switch (value) {
        case 0:
                if (atomic_read(&rmi4_data->st_enabled) == 0)
                        break;

                synaptics_i2c_change_pipe_owner(rmi4_data, APSS);
                synaptics_rmi4_bus_put(rmi4_data);
                atomic_set(&rmi4_data->st_enabled, 0);
                synaptics_secure_touch_notify(rmi4_data);
                complete(&rmi4_data->st_irq_processed);
                synaptics_rmi4_irq(rmi4_data->irq, rmi4_data);
                complete(&rmi4_data->st_powerdown);

                break;
        case 1:
                if (atomic_read(&rmi4_data->st_enabled)) {
                        err = -EBUSY;
                        break;
                }

                synchronize_irq(rmi4_data->irq);

                if (synaptics_rmi4_bus_get(rmi4_data) < 0) {
                        dev_err(
                                rmi4_data->pdev->dev.parent,
                                "synaptics_rmi4_bus_get failed\n");
                        err = -EIO;
                        break;
                }
                synaptics_i2c_change_pipe_owner(rmi4_data, TZ);
                reinit_completion(&rmi4_data->st_powerdown);
                reinit_completion(&rmi4_data->st_irq_processed);
                atomic_set(&rmi4_data->st_enabled, 1);
                atomic_set(&rmi4_data->st_pending_irqs,  0);
                break;
        default:
                dev_err(
                        rmi4_data->pdev->dev.parent,
                        "unsupported value: %lu\n", value);
                err = -EINVAL;
                break;
        }
        return err;
}

/*
 * This function returns whether there are pending interrupts, or
 * other error conditions that need to be signaled to the userspace library,
 * according tot he following logic:
 * - st_enabled is 0 if secure touch is not enabled, returning -EBADF
 * - st_pending_irqs is -1 to signal that secure touch is in being stopped,
 *   returning -EINVAL
 * - st_pending_irqs is 1 to signal that there is a pending irq, returning
 *   the value "1" to the sysfs read operation
 * - st_pending_irqs is 0 (only remaining case left) if the pending interrupt
 *   has been processed, so the interrupt handler can be allowed to continue.
 */
static ssize_t synaptics_secure_touch_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
        struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);
        int val = 0;
        if (atomic_read(&rmi4_data->st_enabled) == 0)
                return -EBADF;

        if (atomic_cmpxchg(&rmi4_data->st_pending_irqs, -1, 0) == -1)
                return -EINVAL;

        if (atomic_cmpxchg(&rmi4_data->st_pending_irqs, 1, 0) == 1)
                val = 1;
        else
                complete(&rmi4_data->st_irq_processed);

        return scnprintf(buf, PAGE_SIZE, "%u", val);

}
#endif

static void synaptics_rmi4_set_intr_mask(struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count)
{
	unsigned char ii;
	unsigned char intr_offset;

	fhandler->intr_reg_num = (intr_count + 7) / 8;
	if (fhandler->intr_reg_num != 0)
		fhandler->intr_reg_num -= 1;

	/* Set an enable bit for each data source */
	intr_offset = intr_count % 8;
	fhandler->intr_mask = 0;
	for (ii = intr_offset;
			ii < (fd->intr_src_count + intr_offset);
			ii++)
		fhandler->intr_mask |= 1 << ii;

	return;
}

static int synaptics_rmi4_f01_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count)
{
	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->data = NULL;
	fhandler->extra = NULL;

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	rmi4_data->f01_query_base_addr = fd->query_base_addr;
	rmi4_data->f01_ctrl_base_addr = fd->ctrl_base_addr;
	rmi4_data->f01_data_base_addr = fd->data_base_addr;
	rmi4_data->f01_cmd_base_addr = fd->cmd_base_addr;

	return 0;
}

static int synaptics_rmi4_f11_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count)
{
	int retval;
	int temp;
	unsigned char offset;
	unsigned char fingers_supported;
	struct synaptics_rmi4_f11_extra_data *extra_data;
	struct synaptics_rmi4_f11_query_0_5 query_0_5;
	struct synaptics_rmi4_f11_query_7_8 query_7_8;
	struct synaptics_rmi4_f11_query_9 query_9;
	struct synaptics_rmi4_f11_query_12 query_12;
	struct synaptics_rmi4_f11_query_27 query_27;
	struct synaptics_rmi4_f11_ctrl_6_9 control_6_9;
	const struct synaptics_dsx_board_data *bdata =
				rmi4_data->hw_if->board_data;

	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->extra = kmalloc(sizeof(*extra_data), GFP_KERNEL);
	if (!fhandler->extra) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for fhandler->extra\n",
				__func__);
		return -ENOMEM;
	}
	extra_data = (struct synaptics_rmi4_f11_extra_data *)fhandler->extra;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base,
			query_0_5.data,
			sizeof(query_0_5.data));
	if (retval < 0)
		return retval;

	/* Maximum number of fingers supported */
	if (query_0_5.num_of_fingers <= 4)
		fhandler->num_of_data_points = query_0_5.num_of_fingers + 1;
	else if (query_0_5.num_of_fingers == 5)
		fhandler->num_of_data_points = 10;

	rmi4_data->num_of_fingers = fhandler->num_of_data_points;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.ctrl_base + 6,
			control_6_9.data,
			sizeof(control_6_9.data));
	if (retval < 0)
		return retval;

	/* Maximum x and y */
	rmi4_data->sensor_max_x = control_6_9.sensor_max_x_pos_7_0 |
			(control_6_9.sensor_max_x_pos_11_8 << 8);
	rmi4_data->sensor_max_y = control_6_9.sensor_max_y_pos_7_0 |
			(control_6_9.sensor_max_y_pos_11_8 << 8);
	dev_dbg(rmi4_data->pdev->dev.parent,
			"%s: Function %02x max x = %d max y = %d\n",
			__func__, fhandler->fn_number,
			rmi4_data->sensor_max_x,
			rmi4_data->sensor_max_y);

	rmi4_data->max_touch_width = MAX_F11_TOUCH_WIDTH;

	if (bdata->swap_axes) {
		temp = rmi4_data->sensor_max_x;
		rmi4_data->sensor_max_x = rmi4_data->sensor_max_y;
		rmi4_data->sensor_max_y = temp;
	}

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	fhandler->data = NULL;

	offset = sizeof(query_0_5.data);

	/* query 6 */
	if (query_0_5.has_rel)
		offset += 1;

	/* queries 7 8 */
	if (query_0_5.has_gestures) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.query_base + offset,
				query_7_8.data,
				sizeof(query_7_8.data));
		if (retval < 0)
			return retval;

		offset += sizeof(query_7_8.data);
	}

	/* query 9 */
	if (query_0_5.has_query_9) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.query_base + offset,
				query_9.data,
				sizeof(query_9.data));
		if (retval < 0)
			return retval;

		offset += sizeof(query_9.data);
	}

	/* query 10 */
	if (query_0_5.has_gestures && query_7_8.has_touch_shapes)
		offset += 1;

	/* query 11 */
	if (query_0_5.has_query_11)
		offset += 1;

	/* query 12 */
	if (query_0_5.has_query_12) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.query_base + offset,
				query_12.data,
				sizeof(query_12.data));
		if (retval < 0)
			return retval;

		offset += sizeof(query_12.data);
	}

	/* query 13 */
	if (query_0_5.has_jitter_filter)
		offset += 1;

	/* query 14 */
	if (query_0_5.has_query_12 && query_12.has_general_information_2)
		offset += 1;

	/* queries 15 16 17 18 19 20 21 22 23 24 25 26*/
	if (query_0_5.has_query_12 && query_12.has_physical_properties)
		offset += 12;

	/* query 27 */
	if (query_0_5.has_query_27) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.query_base + offset,
				query_27.data,
				sizeof(query_27.data));
		if (retval < 0)
			return retval;

		rmi4_data->f11_wakeup_gesture = query_27.has_wakeup_gesture;
	}

	if (!rmi4_data->f11_wakeup_gesture)
		return retval;

	/* data 0 */
	fingers_supported = fhandler->num_of_data_points;
	offset = (fingers_supported + 3) / 4;

	/* data 1 2 3 4 5 */
	offset += 5 * fingers_supported;

	/* data 6 7 */
	if (query_0_5.has_rel)
		offset += 2 * fingers_supported;

	/* data 8 */
	if (query_0_5.has_gestures && query_7_8.data[0])
		offset += 1;

	/* data 9 */
	if (query_0_5.has_gestures && (query_7_8.data[0] || query_7_8.data[1]))
		offset += 1;

	/* data 10 */
	if (query_0_5.has_gestures &&
			(query_7_8.has_pinch || query_7_8.has_flick))
		offset += 1;

	/* data 11 12 */
	if (query_0_5.has_gestures &&
			(query_7_8.has_flick || query_7_8.has_rotate))
		offset += 2;

	/* data 13 */
	if (query_0_5.has_gestures && query_7_8.has_touch_shapes)
		offset += (fingers_supported + 3) / 4;

	/* data 14 15 */
	if (query_0_5.has_gestures &&
			(query_7_8.has_scroll_zones ||
			query_7_8.has_multi_finger_scroll ||
			query_7_8.has_chiral_scroll))
		offset += 2;

	/* data 16 17 */
	if (query_0_5.has_gestures &&
			(query_7_8.has_scroll_zones &&
			query_7_8.individual_scroll_zones))
		offset += 2;

	/* data 18 19 20 21 22 23 24 25 26 27 */
	if (query_0_5.has_query_9 && query_9.has_contact_geometry)
		offset += 10 * fingers_supported;

	/* data 28 */
	if (query_0_5.has_bending_correction ||
			query_0_5.has_large_object_suppression)
		offset += 1;

	/* data 29 30 31 */
	if (query_0_5.has_query_9 && query_9.has_pen_hover_discrimination)
		offset += 3;

	/* data 32 */
	if (query_0_5.has_query_12 &&
			query_12.has_small_object_detection_tuning)
		offset += 1;

	/* data 33 34 */
	if (query_0_5.has_query_27 && query_27.f11_query27_b0)
		offset += 2;

	/* data 35 */
	if (query_0_5.has_query_12 && query_12.has_8bit_w)
		offset += fingers_supported;

	/* data 36 */
	if (query_0_5.has_bending_correction)
		offset += 1;

	/* data 37 */
	if (query_0_5.has_query_27 && query_27.has_data_37)
		offset += 1;

	/* data 38 */
	if (query_0_5.has_query_27 && query_27.has_wakeup_gesture)
		extra_data->data38_offset = offset;

	return retval;
}

static int synaptics_rmi4_f12_set_enables(struct synaptics_rmi4_data *rmi4_data,
		unsigned short ctrl28)
{
	int retval;
	static unsigned short ctrl_28_address;

	if (ctrl28)
		ctrl_28_address = ctrl28;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			ctrl_28_address,
			&rmi4_data->report_enable,
			sizeof(rmi4_data->report_enable));
	if (retval < 0)
		return retval;

	return retval;
}

static int synaptics_rmi4_f12_find_sub(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		unsigned char *presence, unsigned char presence_size,
		unsigned char structure_offset, unsigned char reg,
		unsigned char sub)
{
	int retval;
	unsigned char cnt;
	unsigned char regnum;
	unsigned char bitnum;
	unsigned char p_index;
	unsigned char s_index;
	unsigned char offset;
	unsigned char max_reg;
	unsigned char *structure;

	max_reg = (presence_size - 1) * 8 - 1;

	if (reg > max_reg) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Register number (%d) over limit\n",
				__func__, reg);
		return -EINVAL;
	}

	p_index = reg / 8 + 1;
	bitnum = reg % 8;
	if ((presence[p_index] & (1 << bitnum)) == 0x00) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Register %d is not present\n",
				__func__, reg);
		return -EINVAL;
	}

	structure = kmalloc(presence[0], GFP_KERNEL);
	if (!structure) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for structure register\n",
				__func__);
		return -ENOMEM;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base + structure_offset,
			structure,
			presence[0]);
	if (retval < 0)
		goto exit;

	s_index = 0;

	for (regnum = 0; regnum < reg; regnum++) {
		p_index = regnum / 8 + 1;
		bitnum = regnum % 8;
		if ((presence[p_index] & (1 << bitnum)) == 0x00)
			continue;

		if (structure[s_index] == 0x00)
			s_index += 3;
		else
			s_index++;

		while (structure[s_index] & ~MASK_7BIT)
			s_index++;

		s_index++;
	}

	cnt = 0;
	s_index++;
	offset = sub / 7;
	bitnum = sub % 7;

	do {
		if (cnt == offset) {
			if (structure[s_index + cnt] & (1 << bitnum))
				retval = 1;
			else
				retval = 0;
			goto exit;
		}
		cnt++;
	} while (structure[s_index + cnt - 1] & ~MASK_7BIT);

	retval = 0;

exit:
	kfree(structure);

	return retval;
}

static int synaptics_rmi4_f12_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count)
{
	int retval = 0;
	int temp;
	unsigned char subpacket;
	unsigned char ctrl_23_size;
	unsigned char size_of_2d_data;
	unsigned char size_of_query5;
	unsigned char size_of_query8;
	unsigned char ctrl_8_offset;
#ifdef HTC_FEATURE
	unsigned char ctrl_9_offset;
	unsigned char ctrl_10_offset;
	unsigned char ctrl_11_offset;
	unsigned char ctrl_15_offset;
	unsigned char ctrl_56_offset;
#endif
	unsigned char ctrl_20_offset;
	unsigned char ctrl_23_offset;
	unsigned char ctrl_28_offset;
	unsigned char ctrl_31_offset;
	unsigned char ctrl_58_offset;
	unsigned char num_of_fingers;
	struct synaptics_rmi4_f12_extra_data *extra_data;
	struct synaptics_rmi4_f12_query_5 *query_5 = NULL;
	struct synaptics_rmi4_f12_query_8 *query_8 = NULL;
	struct synaptics_rmi4_f12_ctrl_8 *ctrl_8 = NULL;
	struct synaptics_rmi4_f12_ctrl_23 *ctrl_23 = NULL;
	struct synaptics_rmi4_f12_ctrl_31 *ctrl_31 = NULL;
	struct synaptics_rmi4_f12_ctrl_58 *ctrl_58 = NULL;
	const struct synaptics_dsx_board_data *bdata =
				rmi4_data->hw_if->board_data;

	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->extra = kmalloc(sizeof(*extra_data), GFP_KERNEL);
	if (!fhandler->extra) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for fhandler->extra\n",
				__func__);
		return -ENOMEM;
	}
	extra_data = (struct synaptics_rmi4_f12_extra_data *)fhandler->extra;
	size_of_2d_data = sizeof(struct synaptics_rmi4_f12_finger_data);

	query_5 = kzalloc(sizeof(*query_5), GFP_KERNEL);
	if (!query_5) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for query_5\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

	query_8 = kzalloc(sizeof(*query_8), GFP_KERNEL);
	if (!query_8) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for query_8\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

	ctrl_8 = kzalloc(sizeof(*ctrl_8), GFP_KERNEL);
	if (!ctrl_8) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for ctrl_8\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

	ctrl_23 = kzalloc(sizeof(*ctrl_23), GFP_KERNEL);
	if (!ctrl_23) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for ctrl_23\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

	ctrl_31 = kzalloc(sizeof(*ctrl_31), GFP_KERNEL);
	if (!ctrl_31) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for ctrl_31\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

	ctrl_58 = kzalloc(sizeof(*ctrl_58), GFP_KERNEL);
	if (!ctrl_58) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for ctrl_58\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base + 4,
			&size_of_query5,
			sizeof(size_of_query5));
	if (retval < 0)
		goto exit;

	if (size_of_query5 > sizeof(query_5->data))
		size_of_query5 = sizeof(query_5->data);
	memset(query_5->data, 0x00, sizeof(query_5->data));

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base + 5,
			query_5->data,
			size_of_query5);
	if (retval < 0)
		goto exit;

	ctrl_8_offset = query_5->ctrl0_is_present +
			query_5->ctrl1_is_present +
			query_5->ctrl2_is_present +
			query_5->ctrl3_is_present +
			query_5->ctrl4_is_present +
			query_5->ctrl5_is_present +
			query_5->ctrl6_is_present +
			query_5->ctrl7_is_present;

#ifdef HTC_FEATURE
	ctrl_9_offset = ctrl_8_offset +
			query_5->ctrl8_is_present;

	ctrl_10_offset = ctrl_9_offset +
			query_5->ctrl9_is_present;

	ctrl_11_offset = ctrl_10_offset +
			query_5->ctrl10_is_present;

	ctrl_15_offset = ctrl_11_offset +
			query_5->ctrl11_is_present +
			query_5->ctrl12_is_present +
			query_5->ctrl13_is_present +
			query_5->ctrl14_is_present;
#endif

	ctrl_20_offset = ctrl_8_offset +
			query_5->ctrl8_is_present +
			query_5->ctrl9_is_present +
			query_5->ctrl10_is_present +
			query_5->ctrl11_is_present +
			query_5->ctrl12_is_present +
			query_5->ctrl13_is_present +
			query_5->ctrl14_is_present +
			query_5->ctrl15_is_present +
			query_5->ctrl16_is_present +
			query_5->ctrl17_is_present +
			query_5->ctrl18_is_present +
			query_5->ctrl19_is_present;

	ctrl_23_offset = ctrl_20_offset +
			query_5->ctrl20_is_present +
			query_5->ctrl21_is_present +
			query_5->ctrl22_is_present;

	ctrl_28_offset = ctrl_23_offset +
			query_5->ctrl23_is_present +
			query_5->ctrl24_is_present +
			query_5->ctrl25_is_present +
			query_5->ctrl26_is_present +
			query_5->ctrl27_is_present;

	ctrl_31_offset = ctrl_28_offset +
			query_5->ctrl28_is_present +
			query_5->ctrl29_is_present +
			query_5->ctrl30_is_present;

#ifdef HTC_FEATURE
	ctrl_56_offset = ctrl_31_offset +
			query_5->ctrl31_is_present +
			query_5->ctrl32_is_present +
			query_5->ctrl33_is_present +
			query_5->ctrl34_is_present +
			query_5->ctrl35_is_present +
			query_5->ctrl36_is_present +
			query_5->ctrl37_is_present +
			query_5->ctrl38_is_present +
			query_5->ctrl39_is_present +
			query_5->ctrl40_is_present +
			query_5->ctrl41_is_present +
			query_5->ctrl42_is_present +
			query_5->ctrl43_is_present +
			query_5->ctrl44_is_present +
			query_5->ctrl45_is_present +
			query_5->ctrl46_is_present +
			query_5->ctrl47_is_present +
			query_5->ctrl48_is_present +
			query_5->ctrl49_is_present +
			query_5->ctrl50_is_present +
			query_5->ctrl51_is_present +
			query_5->ctrl52_is_present +
			query_5->ctrl53_is_present +
			query_5->ctrl54_is_present +
			query_5->ctrl55_is_present;

	ctrl_58_offset = ctrl_56_offset +
			query_5->ctrl56_is_present +
			query_5->ctrl57_is_present;

#else
	ctrl_58_offset = ctrl_31_offset +
			query_5->ctrl31_is_present +
			query_5->ctrl32_is_present +
			query_5->ctrl33_is_present +
			query_5->ctrl34_is_present +
			query_5->ctrl35_is_present +
			query_5->ctrl36_is_present +
			query_5->ctrl37_is_present +
			query_5->ctrl38_is_present +
			query_5->ctrl39_is_present +
			query_5->ctrl40_is_present +
			query_5->ctrl41_is_present +
			query_5->ctrl42_is_present +
			query_5->ctrl43_is_present +
			query_5->ctrl44_is_present +
			query_5->ctrl45_is_present +
			query_5->ctrl46_is_present +
			query_5->ctrl47_is_present +
			query_5->ctrl48_is_present +
			query_5->ctrl49_is_present +
			query_5->ctrl50_is_present +
			query_5->ctrl51_is_present +
			query_5->ctrl52_is_present +
			query_5->ctrl53_is_present +
			query_5->ctrl54_is_present +
			query_5->ctrl55_is_present +
			query_5->ctrl56_is_present +
			query_5->ctrl57_is_present;
#endif

	ctrl_23_size = 2;
	for (subpacket = 2; subpacket <= 4; subpacket++) {
		retval = synaptics_rmi4_f12_find_sub(rmi4_data,
				fhandler, query_5->data, sizeof(query_5->data),
				6, 23, subpacket);
		if (retval == 1)
			ctrl_23_size++;
		else if (retval < 0)
			goto exit;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.ctrl_base + ctrl_23_offset,
			ctrl_23->data,
			ctrl_23_size);
	if (retval < 0)
		goto exit;

	/* Maximum number of fingers supported */
	fhandler->num_of_data_points = min_t(unsigned char,
			ctrl_23->max_reported_objects,
			(unsigned char)F12_FINGERS_TO_SUPPORT);

	num_of_fingers = fhandler->num_of_data_points;
	rmi4_data->num_of_fingers = num_of_fingers;

	rmi4_data->stylus_enable = ctrl_23->stylus_enable;
	rmi4_data->eraser_enable = ctrl_23->eraser_enable;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base + 7,
			&size_of_query8,
			sizeof(size_of_query8));
	if (retval < 0)
		goto exit;

	if (size_of_query8 > sizeof(query_8->data))
		size_of_query8 = sizeof(query_8->data);
	memset(query_8->data, 0x00, sizeof(query_8->data));

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base + 8,
			query_8->data,
			size_of_query8);
	if (retval < 0)
		goto exit;

	/* Determine the presence of the Data0 register */
	extra_data->data1_offset = query_8->data0_is_present;

	if ((size_of_query8 >= 3) && (query_8->data15_is_present)) {
		extra_data->data15_offset = query_8->data0_is_present +
				query_8->data1_is_present +
				query_8->data2_is_present +
				query_8->data3_is_present +
				query_8->data4_is_present +
				query_8->data5_is_present +
				query_8->data6_is_present +
				query_8->data7_is_present +
				query_8->data8_is_present +
				query_8->data9_is_present +
				query_8->data10_is_present +
				query_8->data11_is_present +
				query_8->data12_is_present +
				query_8->data13_is_present +
				query_8->data14_is_present;
		extra_data->data15_size = (num_of_fingers + 7) / 8;
	} else {
		extra_data->data15_size = 0;
	}

#ifdef REPORT_2D_PRESSURE
	if ((size_of_query8 >= 5) && (query_8->data29_is_present)) {
		extra_data->data29_offset = query_8->data0_is_present +
				query_8->data1_is_present +
				query_8->data2_is_present +
				query_8->data3_is_present +
				query_8->data4_is_present +
				query_8->data5_is_present +
				query_8->data6_is_present +
				query_8->data7_is_present +
				query_8->data8_is_present +
				query_8->data9_is_present +
				query_8->data10_is_present +
				query_8->data11_is_present +
				query_8->data12_is_present +
				query_8->data13_is_present +
				query_8->data14_is_present +
				query_8->data15_is_present +
				query_8->data16_is_present +
				query_8->data17_is_present +
				query_8->data18_is_present +
				query_8->data19_is_present +
				query_8->data20_is_present +
				query_8->data21_is_present +
				query_8->data22_is_present +
				query_8->data23_is_present +
				query_8->data24_is_present +
				query_8->data25_is_present +
				query_8->data26_is_present +
				query_8->data27_is_present +
				query_8->data28_is_present;
		extra_data->data29_size = 0;
		for (subpacket = 0; subpacket <= num_of_fingers; subpacket++) {
			retval = synaptics_rmi4_f12_find_sub(rmi4_data,
					fhandler, query_8->data,
					sizeof(query_8->data),
					9, 29, subpacket);
			if (retval == 1)
				extra_data->data29_size += 2;
			else if (retval < 0)
				goto exit;
		}
		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.ctrl_base + ctrl_58_offset,
				ctrl_58->data,
				sizeof(ctrl_58->data));
		if (retval < 0)
			goto exit;
		rmi4_data->force_min =
				(int)(ctrl_58->min_force_lsb << 0) |
				(int)(ctrl_58->min_force_msb << 8);
		rmi4_data->force_max =
				(int)(ctrl_58->max_force_lsb << 0) |
				(int)(ctrl_58->max_force_msb << 8);
		rmi4_data->report_pressure = true;
	} else {
		extra_data->data29_size = 0;
		rmi4_data->report_pressure = false;
	}
#endif

	rmi4_data->report_enable = RPT_DEFAULT;
#ifdef REPORT_2D_Z
	rmi4_data->report_enable |= RPT_Z;
#endif
#ifdef REPORT_2D_W
	rmi4_data->report_enable |= (RPT_WX | RPT_WY);
#endif

	retval = synaptics_rmi4_f12_set_enables(rmi4_data,
			fhandler->full_addr.ctrl_base + ctrl_28_offset);
	if (retval < 0)
		goto exit;

	if (query_5->ctrl8_is_present) {
		rmi4_data->wedge_sensor = false;

		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.ctrl_base + ctrl_8_offset,
				ctrl_8->data,
				sizeof(ctrl_8->data));
		if (retval < 0)
			goto exit;

		/* Maximum x and y */
		rmi4_data->sensor_max_x =
				((unsigned int)ctrl_8->max_x_coord_lsb << 0) |
				((unsigned int)ctrl_8->max_x_coord_msb << 8);
		rmi4_data->sensor_max_y =
				((unsigned int)ctrl_8->max_y_coord_lsb << 0) |
				((unsigned int)ctrl_8->max_y_coord_msb << 8);

		rmi4_data->max_touch_width = MAX_F12_TOUCH_WIDTH;

#ifdef HTC_FEATURE
		rmi4_data->num_of_rx = ctrl_8->num_of_rx;
		rmi4_data->num_of_tx = ctrl_8->num_of_tx;

		dev_dbg(rmi4_data->pdev->dev.parent,
				"%s: Function %02x rx num %d tx num %d\n",
				__func__, fhandler->fn_number,
				rmi4_data->num_of_rx,
				rmi4_data->num_of_tx);
#endif
	} else {
		rmi4_data->wedge_sensor = true;

		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.ctrl_base + ctrl_31_offset,
				ctrl_31->data,
				sizeof(ctrl_31->data));
		if (retval < 0)
			goto exit;

		/* Maximum x and y */
		rmi4_data->sensor_max_x =
				((unsigned int)ctrl_31->max_x_coord_lsb << 0) |
				((unsigned int)ctrl_31->max_x_coord_msb << 8);
		rmi4_data->sensor_max_y =
				((unsigned int)ctrl_31->max_y_coord_lsb << 0) |
				((unsigned int)ctrl_31->max_y_coord_msb << 8);

		rmi4_data->max_touch_width = MAX_F12_TOUCH_WIDTH;
	}

#ifdef HTC_FEATURE
	if (bdata->display_width && bdata->display_height) {
		rmi4_data->factor_width = (bdata->display_width << RESOL_SHIFT_BITS) / rmi4_data->sensor_max_x;
		rmi4_data->factor_height = (bdata->display_height << RESOL_SHIFT_BITS) / rmi4_data->sensor_max_y;
	}
#endif

	dev_dbg(rmi4_data->pdev->dev.parent,
			"%s: Function %02x max x = %d max y = %d\n",
			__func__, fhandler->fn_number,
			rmi4_data->sensor_max_x,
			rmi4_data->sensor_max_y);

	if (bdata->swap_axes) {
		temp = rmi4_data->sensor_max_x;
		rmi4_data->sensor_max_x = rmi4_data->sensor_max_y;
		rmi4_data->sensor_max_y = temp;
	}

	rmi4_data->f12_wakeup_gesture = query_5->ctrl27_is_present;
	if (rmi4_data->f12_wakeup_gesture) {
		extra_data->ctrl20_offset = ctrl_20_offset;
		extra_data->data4_offset = query_8->data0_is_present +
				query_8->data1_is_present +
				query_8->data2_is_present +
				query_8->data3_is_present;
	}

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	/* Allocate memory for finger data storage space */
	fhandler->data_size = num_of_fingers * size_of_2d_data;
	fhandler->data = kmalloc(fhandler->data_size, GFP_KERNEL);
	if (!fhandler->data) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for fhandler->data\n",
				__func__);
		retval = -ENOMEM;
		goto exit;
	}

#ifdef HTC_FEATURE
	extra_data->ctrl10_offset = ctrl_10_offset;
	extra_data->ctrl15_offset = ctrl_15_offset;
	extra_data->ctrl23_offset = ctrl_23_offset;
	extra_data->ctrl56_offset = ctrl_56_offset;
#endif

exit:
	kfree(query_5);
	kfree(query_8);
	kfree(ctrl_8);
	kfree(ctrl_23);
	kfree(ctrl_31);
	kfree(ctrl_58);

	return retval;
}

static int synaptics_rmi4_f1a_alloc_mem(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	struct synaptics_rmi4_f1a_handle *f1a;

	f1a = kzalloc(sizeof(*f1a), GFP_KERNEL);
	if (!f1a) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for function handle\n",
				__func__);
		return -ENOMEM;
	}

	fhandler->data = (void *)f1a;
	fhandler->extra = NULL;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			fhandler->full_addr.query_base,
			f1a->button_query.data,
			sizeof(f1a->button_query.data));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read query registers\n",
				__func__);
		return retval;
	}

	f1a->max_count = f1a->button_query.max_button_count + 1;

	f1a->button_control.txrx_map = kzalloc(f1a->max_count * 2, GFP_KERNEL);
	if (!f1a->button_control.txrx_map) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for tx rx mapping\n",
				__func__);
		return -ENOMEM;
	}

	f1a->button_bitmask_size = (f1a->max_count + 7) / 8;

	f1a->button_data_buffer = kcalloc(f1a->button_bitmask_size,
			sizeof(*(f1a->button_data_buffer)), GFP_KERNEL);
	if (!f1a->button_data_buffer) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for data buffer\n",
				__func__);
		return -ENOMEM;
	}

	f1a->button_map = kcalloc(f1a->max_count,
			sizeof(*(f1a->button_map)), GFP_KERNEL);
	if (!f1a->button_map) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for button map\n",
				__func__);
		return -ENOMEM;
	}

	return 0;
}

static int synaptics_rmi4_f1a_button_map(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler)
{
	int retval;
	unsigned char ii;
	unsigned char offset = 0;
	struct synaptics_rmi4_f1a_query_4 query_4;
	struct synaptics_rmi4_f1a_handle *f1a = fhandler->data;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	rmi4_data->valid_button_count = f1a->valid_button_count;

	offset = f1a->button_query.has_general_control +
			f1a->button_query.has_interrupt_enable +
			f1a->button_query.has_multibutton_select;

	if (f1a->button_query.has_tx_rx_map) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.ctrl_base + offset,
				f1a->button_control.txrx_map,
				f1a->max_count * 2);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to read tx rx mapping\n",
					__func__);
			return retval;
		}

		rmi4_data->button_txrx_mapping = f1a->button_control.txrx_map;
	}

	if (f1a->button_query.has_query4) {
		offset = 2 + f1a->button_query.has_query2 +
				f1a->button_query.has_query3;

		retval = synaptics_rmi4_reg_read(rmi4_data,
				fhandler->full_addr.query_base + offset,
				query_4.data,
				sizeof(query_4.data));
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to read button features 4\n",
					__func__);
			return retval;
		}

		if (query_4.has_ctrl24)
			rmi4_data->external_afe_buttons = true;
		else
			rmi4_data->external_afe_buttons = false;
	}

	if (!bdata->cap_button_map) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: cap_button_map is NULL in board file\n",
				__func__);
		return -ENODEV;
	} else if (!bdata->cap_button_map->map) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Button map is missing in board file\n",
				__func__);
		return -ENODEV;
	} else {
		if (bdata->cap_button_map->nbuttons != f1a->max_count) {
			f1a->valid_button_count = min(f1a->max_count,
					bdata->cap_button_map->nbuttons);
		} else {
			f1a->valid_button_count = f1a->max_count;
		}

		for (ii = 0; ii < f1a->valid_button_count; ii++)
			f1a->button_map[ii] = bdata->cap_button_map->map[ii];

		rmi4_data->valid_button_count = f1a->valid_button_count;
	}

	return 0;
}

static void synaptics_rmi4_f1a_kfree(struct synaptics_rmi4_fn *fhandler)
{
	struct synaptics_rmi4_f1a_handle *f1a = fhandler->data;

	if (f1a) {
		kfree(f1a->button_control.txrx_map);
		kfree(f1a->button_data_buffer);
		kfree(f1a->button_map);
		kfree(f1a);
		fhandler->data = NULL;
	}

	return;
}

static int synaptics_rmi4_f1a_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count)
{
	int retval;

	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	retval = synaptics_rmi4_f1a_alloc_mem(rmi4_data, fhandler);
	if (retval < 0)
		goto error_exit;

	retval = synaptics_rmi4_f1a_button_map(rmi4_data, fhandler);
	if (retval < 0)
		goto error_exit;

	rmi4_data->button_0d_enabled = 1;

	return 0;

error_exit:
	synaptics_rmi4_f1a_kfree(fhandler);

	return retval;
}

#ifdef HTC_FEATURE
static int synaptics_rmi4_f34_v7_queries(struct synaptics_rmi4_data *rmi4_data)
{
	struct f34_v7_query_0 query_0;
	struct f34_v7_query_1_7 query_1_7;
	unsigned char offset;
	int retval;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f34_query_base_addr,
			query_0.data,
			sizeof(query_0.data));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read query 0\n",
				__func__);
		return retval;
	}

	offset = query_0.subpacket_1_size + 1;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f34_query_base_addr + offset,
			query_1_7.data,
			sizeof(query_1_7.data));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read queries 1 to 7\n",
				__func__);
		return retval;
	}

	if (query_1_7.bl_major_revision == BL_V8)
		rmi4_data->bl_version = BL_V8;

	return 0;
}

static int synaptics_rmi4_f34_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count,
		unsigned int page_number)
{
	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->data = NULL;
	fhandler->extra = NULL;

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	rmi4_data->f34_query_base_addr =
		(fd->query_base_addr | (page_number << 8));
	rmi4_data->f34_ctrl_base_addr =
		(fd->ctrl_base_addr | (page_number << 8));
	rmi4_data->f34_data_base_addr =
		(fd->data_base_addr | (page_number << 8));
	rmi4_data->f34_cmd_base_addr =
		(fd->cmd_base_addr | (page_number << 8));

	switch (fd->fn_version) {
		case F34_V0:
			rmi4_data->bl_version = BL_V5;
			break;
		case F34_V1:
			rmi4_data->bl_version = BL_V6;
			break;
		case F34_V2:
			rmi4_data->bl_version = BL_V7;
			synaptics_rmi4_f34_v7_queries(rmi4_data);
			break;
		default:
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Unrecognized F34 version\n",
					__func__);
			return -EINVAL;
	}

	return 0;
}

static int synaptics_rmi4_f51_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count,
		unsigned int page_number)
{
	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->data = NULL;
	fhandler->extra = NULL;

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	rmi4_data->f51_query_base_addr =
		(fd->query_base_addr | (page_number << 8));
	rmi4_data->f51_ctrl_base_addr =
		(fd->ctrl_base_addr | (page_number << 8));
	rmi4_data->f51_data_base_addr =
		(fd->data_base_addr | (page_number << 8));
	rmi4_data->f51_cmd_base_addr =
		(fd->cmd_base_addr | (page_number << 8));

	return 0;
}

static int synaptics_rmi4_f54_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count,
		unsigned int page_number)
{
	struct synaptics_rmi4_f54_query f54_query;
	struct synaptics_rmi4_f54_query_13 query_13;
	struct synaptics_rmi4_f54_query_15 query_15;
	struct synaptics_rmi4_f54_query_16 query_16;
	struct synaptics_rmi4_f54_query_21 query_21;
	struct synaptics_rmi4_f54_query_22 query_22;
	struct synaptics_rmi4_f54_query_23 query_23;
	struct synaptics_rmi4_f54_query_25 query_25;
	struct synaptics_rmi4_f54_query_27 query_27;
	struct synaptics_rmi4_f54_query_29 query_29;
	struct synaptics_rmi4_f54_query_30 query_30;
	struct synaptics_rmi4_f54_query_32 query_32;
	struct synaptics_rmi4_f54_query_33 query_33;
	struct synaptics_rmi4_f54_query_35 query_35;
	struct synaptics_rmi4_f54_query_36 query_36;
	struct synaptics_rmi4_f54_query_38 query_38;
	struct synaptics_rmi4_f54_query_39 query_39;
	struct synaptics_rmi4_f54_query_40 query_40;
	struct synaptics_rmi4_f54_query_43 query_43;
	struct synaptics_rmi4_f54_query_46 query_46;
	struct synaptics_rmi4_f54_query_47 query_47;
	struct synaptics_rmi4_f54_query_49 query_49;
	struct synaptics_rmi4_f54_query_50 query_50;
	struct synaptics_rmi4_f54_query_51 query_51;
	struct synaptics_rmi4_f54_query_55 query_55;
	struct synaptics_rmi4_f54_query_57 query_57;
	struct synaptics_rmi4_f54_query_58 query_58;
	struct synaptics_rmi4_f54_query_61 query_61;
	struct synaptics_rmi4_f54_query_64 query_64;
	struct synaptics_rmi4_f54_query_65 query_65;
	struct synaptics_rmi4_f54_query_67 query_67;
	struct synaptics_rmi4_f54_query_68 query_68;
	unsigned char data_offset = REPORT_DATA_OFFSET;
	unsigned char offset;
	unsigned char ctrl_offset = 0;
	unsigned char num_of_sensing_freqs;
	unsigned char length;
	int retval;

	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->data = NULL;
	fhandler->extra = NULL;

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	rmi4_data->f54_query_base_addr =
		(fd->query_base_addr | (page_number << 8));
	rmi4_data->f54_ctrl_base_addr =
		(fd->ctrl_base_addr | (page_number << 8));
	rmi4_data->f54_data_base_addr =
		(fd->data_base_addr | (page_number << 8));
	rmi4_data->f54_cmd_base_addr =
		(fd->cmd_base_addr | (page_number << 8));

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f54_query_base_addr,
			f54_query.data,
			sizeof(f54_query));
	if (retval < 0)
		return retval;

	offset = sizeof(f54_query.data);

	/* query 12 */
	if (f54_query.has_sense_frequency_control == 0)
		offset -= 1;

	/* query 13 */
	if (f54_query.has_query13) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_13.data,
				sizeof(query_13.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 14 */
	if (query_13.has_ctrl87)
		offset += 1;

	/* query 15 */
	if (f54_query.has_query15) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_15.data,
				sizeof(query_15.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 16 */
	if (query_15.has_query16) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_16.data,
				sizeof(query_16.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 17 */
	if (query_16.has_query17)
		offset += 1;

	/* query 18 */
	if (query_16.has_ctrl94_query18)
		offset += 1;

	/* query 19 */
	if (query_16.has_ctrl95_query19)
		offset += 1;

	/* query 20 */
	if (query_15.has_query20)
		offset += 1;

	/* query 21 */
	if (query_15.has_query21) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_21.data,
				sizeof(query_21.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 22 */
	if (query_15.has_query22) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_22.data,
				sizeof(query_22.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 23 */
	if (query_22.has_query23) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_23.data,
				sizeof(query_23.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 24 */
	if (query_21.has_query24_data18)
		offset += 1;

	/* query 25 */
	if (query_15.has_query25) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_25.data,
				sizeof(query_25.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 26 */
	if (query_22.has_ctrl103_query26)
		offset += 1;

	/* query 27 */
	if (query_25.has_query27) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_27.data,
				sizeof(query_27.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 28 */
	if (query_22.has_query28)
		offset += 1;

	/* query 29 */
	if (query_27.has_query29) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_29.data,
				sizeof(query_29.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 30 */
	if (query_29.has_query30) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_30.data,
				sizeof(query_30.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 31 */
	if (query_30.has_ctrl122_query31)
		offset += 1;

	/* query 32 */
	if (query_30.has_query32) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_32.data,
				sizeof(query_32.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 33 */
	if (query_32.has_query33) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_33.data,
				sizeof(query_33.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 34 */
	if (query_32.has_query34)
		offset += 1;

	/* query 35 */
	if (query_32.has_query35) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_35.data,
				sizeof(query_35.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 36 */
	if (query_33.has_query36) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_36.data,
				sizeof(query_36.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 37 */
	if (query_36.has_query37)
		offset += 1;

	/* query 38 */
	if (query_36.has_query38) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_38.data,
				sizeof(query_38.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 39 */
	if (query_38.has_query39) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_39.data,
				sizeof(query_39.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 40 */
	if (query_39.has_query40) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_40.data,
				sizeof(query_40.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 41 */
	if (query_40.has_ctrl163_query41)
		offset += 1;

	/* query 42 */
	if (query_40.has_ctrl165_query42)
		offset += 1;

	/* query 43 */
	if (query_40.has_query43) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_43.data,
				sizeof(query_43.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	if (query_43.has_ctrl172_query44_query45)
		offset += 2;

	/* query 46 */
	if (query_43.has_query46) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_46.data,
				sizeof(query_46.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 47 */
	if (query_46.has_query47) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_47.data,
				sizeof(query_47.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 48 reserved */

	/* query 49 */
	if (query_47.has_query49) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_49.data,
				sizeof(query_49.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 50 */
	if (query_49.has_query50) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_50.data,
				sizeof(query_50.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 51 */
	if (query_50.has_query51) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_51.data,
				sizeof(query_51.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 53 54 */
	if (query_51.has_query53_query54_ctrl198)
		offset += 2;

	/* query 55 */
	if (query_51.has_query55) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_55.data,
				sizeof(query_55.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 56 */
	if (query_55.has_query56)
		offset += 1;

	/* query 57 */
	if (query_55.has_query57) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_57.data,
				sizeof(query_57.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 58 */
	if (query_57.has_query58) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_58.data,
				sizeof(query_58.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 59 */
	if (query_58.has_query59)
		offset += 1;

	/* query 60 */
	if (query_58.has_query60)
		offset += 1;

	/* query 61 */
	if (query_58.has_query61) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_61.data,
				sizeof(query_61.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 62 63 */
	if (query_61.has_ctrl215_query62_query63)
		offset += 2;

	/* query 64 */
	if (query_61.has_query64) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_64.data,
				sizeof(query_64.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 65 */
	if (query_64.has_query65) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_65.data,
				sizeof(query_65.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 66 */
	if (query_65.has_query66_ctrl231)
		offset += 1;

	/* query 67 */
	if (query_65.has_query67) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_67.data,
				sizeof(query_67.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	/* query 68 */
	if (query_67.has_query68) {
		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f54_query_base_addr + offset,
				query_68.data,
				sizeof(query_68.data));
		if (retval < 0)
			return retval;
		offset += 1;
	}

	num_of_sensing_freqs = f54_query.number_of_sensing_frequencies;

	/* F54_CONTROL 0 */
	ctrl_offset += F54_CONTROL_0_SIZE;

	/* F54_CONTROL 1 */
	if ((f54_query.touch_controller_family == 0) ||
			(f54_query.touch_controller_family == 1))
		ctrl_offset += F54_CONTROL_1_SIZE;

	/* F54_CONTROL 2 */
	ctrl_offset += F54_CONTROL_2_SIZE;

	/* F54_CONTROL 3 */
	if (f54_query.has_pixel_touch_threshold_adjustment)
		ctrl_offset += F54_CONTROL_3_SIZE;

	/* F54_CONTROLs 4 5 6 */
	if ((f54_query.touch_controller_family == 0) ||
			(f54_query.touch_controller_family == 1))
		ctrl_offset += F54_CONTROL_4_6_SIZE;

	/* F54_CONTROL 7 */
	if (f54_query.touch_controller_family == 1)
		ctrl_offset += F54_CONTROL_7_SIZE;

	/* F54_CONTROLs 8 9 */
	if ((f54_query.touch_controller_family == 0) ||
			(f54_query.touch_controller_family == 1))
		ctrl_offset += F54_CONTROL_8_9_SIZE;

	/* F54_CONTROL 10 */
	if (f54_query.has_interference_metric)
		ctrl_offset += F54_CONTROL_10_SIZE;

	/* F54_CONTROL 11 */
	if (f54_query.has_ctrl11)
		ctrl_offset += F54_CONTROL_11_SIZE;

	/* F54_CONTROLs 12 13 */
	if (f54_query.has_relaxation_control)
		ctrl_offset += F54_CONTROL_12_13_SIZE;

	/* F54_CONTROLs 14 15 16 */
	if (f54_query.has_sensor_assignment) {
		ctrl_offset += F54_CONTROL_14_SIZE;
		ctrl_offset += F54_CONTROL_15_SIZE * f54_query.num_of_rx_electrodes;
		ctrl_offset += F54_CONTROL_16_SIZE * f54_query.num_of_tx_electrodes;
	}

	/* F54_CONTROLs 17 18 19 */
	if (f54_query.has_sense_frequency_control) {
		ctrl_offset += F54_CONTROL_17_SIZE * num_of_sensing_freqs;
		ctrl_offset += F54_CONTROL_18_SIZE * num_of_sensing_freqs;
		ctrl_offset += F54_CONTROL_19_SIZE * num_of_sensing_freqs;
	}

	/* F54_CONTROL 20 */
	ctrl_offset += F54_CONTROL_20_SIZE;

	/* F54_CONTROL 21 */
	if (f54_query.has_sense_frequency_control)
		ctrl_offset += F54_CONTROL_21_SIZE;

	/* F54_CONTROLs 22 23 24 25 26 */
	if (f54_query.has_firmware_noise_mitigation)
		ctrl_offset += F54_CONTROL_22_26_SIZE;

	/* F54_CONTROL 27 */
	if (f54_query.has_iir_filter)
		ctrl_offset += F54_CONTROL_27_SIZE;

	/* F54_CONTROL 28 */
	if (f54_query.has_firmware_noise_mitigation)
		ctrl_offset += F54_CONTROL_28_SIZE;

	/* F54_CONTROL 29 */
	if (f54_query.has_cmn_removal)
		ctrl_offset += F54_CONTROL_29_SIZE;

	/* F54_CONTROL 30 */
	if (f54_query.has_cmn_maximum)
		ctrl_offset += F54_CONTROL_30_SIZE;

	/* F54_CONTROL 31 */
	if (f54_query.has_touch_hysteresis)
		ctrl_offset += F54_CONTROL_31_SIZE;

	/* F54_CONTROLs 32 33 34 35 */
	if (f54_query.has_edge_compensation)
		ctrl_offset += F54_CONTROL_32_35_SIZE;

	/* F54_CONTROL 36 */
	if ((f54_query.curve_compensation_mode == 1) ||
			(f54_query.curve_compensation_mode == 2)) {
		if (f54_query.curve_compensation_mode == 1) {
			length = max(f54_query.num_of_rx_electrodes,
					f54_query.num_of_tx_electrodes);
		} else if (f54_query.curve_compensation_mode == 2) {
			length = f54_query.num_of_rx_electrodes;
		}
		ctrl_offset += F54_CONTROL_36_SIZE * length;
	}

	/* F54_CONTROL 37 */
	if (f54_query.curve_compensation_mode == 2)
		ctrl_offset += F54_CONTROL_37_SIZE * f54_query.num_of_tx_electrodes;

	/* F54_CONTROLs 38 39 40 */
	if (f54_query.has_per_frequency_noise_control) {
		ctrl_offset += F54_CONTROL_38_SIZE * num_of_sensing_freqs;
		ctrl_offset += F54_CONTROL_39_SIZE * num_of_sensing_freqs;
		ctrl_offset += F54_CONTROL_40_SIZE * num_of_sensing_freqs;
	}

	/* F54_CONTROL 41 */
	if (f54_query.has_signal_clarity)
		ctrl_offset += F54_CONTROL_41_SIZE;

	/* F54_CONTROL 42 */
	if (f54_query.has_variance_metric)
		ctrl_offset += F54_CONTROL_42_SIZE;

	/* F54_CONTROLs 43 44 45 46 47 48 49 50 51 52 53 54 */
	if (f54_query.has_multi_metric_state_machine)
		ctrl_offset += F54_CONTROL_43_54_SIZE;

	/* F54_CONTROLs 55 56 */
	if (f54_query.has_0d_relaxation_control)
		ctrl_offset += F54_CONTROL_55_56_SIZE;

	/* F54_CONTROL 57 */
	if (f54_query.has_0d_acquisition_control)
		ctrl_offset += F54_CONTROL_57_SIZE;

	/* F54_CONTROL 58 */
	if (f54_query.has_0d_acquisition_control)
		ctrl_offset += F54_CONTROL_58_SIZE;

	/* F54_CONTROL 59 */
	if (f54_query.has_h_blank)
		ctrl_offset += F54_CONTROL_59_SIZE;

	/* F54_CONTROLs 60 61 62 */
	if ((f54_query.has_h_blank) ||
			(f54_query.has_v_blank) ||
			(f54_query.has_long_h_blank))
		ctrl_offset += F54_CONTROL_60_62_SIZE;

	/* F54_CONTROL 63 */
	if ((f54_query.has_h_blank) ||
			(f54_query.has_v_blank) ||
			(f54_query.has_long_h_blank) ||
			(f54_query.has_slew_metric) ||
			(f54_query.has_slew_option) ||
			(f54_query.has_noise_mitigation2))
		ctrl_offset += F54_CONTROL_63_SIZE;

	/* F54_CONTROLs 64 65 66 67 */
	if (f54_query.has_h_blank)
		ctrl_offset += F54_CONTROL_64_67_SIZE * 7;
	else if ((f54_query.has_v_blank) ||
			(f54_query.has_long_h_blank))
		ctrl_offset += F54_CONTROL_64_67_SIZE;

	/* F54_CONTROLs 68 69 70 71 72 73 */
	if ((f54_query.has_h_blank) ||
			(f54_query.has_v_blank) ||
			(f54_query.has_long_h_blank)) {
		if (query_68.is_tddi_hic)
			ctrl_offset += F54_CONTROL_70_73_SIZE;
		else
			ctrl_offset += F54_CONTROL_68_73_SIZE;
	}

	/* F54_CONTROL 74 */
	if (f54_query.has_slew_metric)
		ctrl_offset += F54_CONTROL_74_SIZE;

	/* F54_CONTROL 75 */
	if (f54_query.has_enhanced_stretch)
		ctrl_offset += F54_CONTROL_75_SIZE * num_of_sensing_freqs;

	/* F54_CONTROL 76 */
	if (f54_query.has_startup_fast_relaxation)
		ctrl_offset += F54_CONTROL_76_SIZE;

	/* F54_CONTROLs 77 78 */
	if (f54_query.has_esd_control)
		ctrl_offset += F54_CONTROL_77_78_SIZE;

	/* F54_CONTROLs 79 80 81 82 83 */
	if (f54_query.has_noise_mitigation2)
		ctrl_offset += F54_CONTROL_79_83_SIZE;

	/* F54_CONTROLs 84 85 */
	if (f54_query.has_energy_ratio_relaxation)
		ctrl_offset += F54_CONTROL_84_85_SIZE;

	/* F54_CONTROL 86 */
	if (query_13.has_ctrl86)
		ctrl_offset += F54_CONTROL_86_SIZE;

	/* F54_CONTROL 87 */
	if (query_13.has_ctrl87)
		ctrl_offset += F54_CONTROL_87_SIZE;

	/* F54_CONTROL 88 */
	if (f54_query.has_ctrl88)
		ctrl_offset += F54_CONTROL_88_SIZE;

	/* F54_CONTROL 89 */
	if (query_13.has_cidim ||
			query_13.has_noise_mitigation_enhancement ||
			query_13.has_rail_im)
		ctrl_offset += F54_CONTROL_89_SIZE;

	/* F54_CONTROL 90 */
	if (query_15.has_ctrl90)
		ctrl_offset += F54_CONTROL_90_SIZE;

	/* F54_CONTROL 91 */
	if (query_21.has_ctrl91)
		ctrl_offset += F54_CONTROL_91_SIZE;

	/* F54_CONTROL 92 */
	if (query_16.has_ctrl92)
		ctrl_offset += F54_CONTROL_92_SIZE;

	/* F54_CONTROL 93 */
	if (query_16.has_ctrl93)
		ctrl_offset += F54_CONTROL_93_SIZE;

	/* F54_CONTROL 94 */
	if (query_16.has_ctrl94_query18)
		ctrl_offset += F54_CONTROL_94_SIZE;

	/* F54_CONTROL 95 */
	if (query_16.has_ctrl95_query19)
		ctrl_offset += F54_CONTROL_95_SIZE;

	/* F54_CONTROL 96 */
	if (query_21.has_ctrl96)
		ctrl_offset += F54_CONTROL_96_SIZE;

	/* F54_CONTROL 97 */
	if (query_21.has_ctrl97)
		ctrl_offset += F54_CONTROL_97_SIZE;

	/* F54_CONTROL 98 */
	if (query_21.has_ctrl98)
		ctrl_offset += F54_CONTROL_98_SIZE;

	/* F54_CONTROL 99 */
	if (f54_query.touch_controller_family == 2)
		ctrl_offset += F54_CONTROL_99_SIZE;

	/* F54_CONTROL 100 */
	if (query_16.has_ctrl100)
		ctrl_offset += F54_CONTROL_100_SIZE;

	/* F54_CONTROL 101 */
	if (query_22.has_ctrl101)
		ctrl_offset += F54_CONTROL_101_SIZE;

	/* F54_CONTROL 102 */
	if (query_23.has_ctrl102)
		ctrl_offset += F54_CONTROL_102_SIZE;

	/* F54_CONTROL 103 */
	if (query_22.has_ctrl103_query26) {
		ctrl_offset += F54_CONTROL_103_SIZE;
	}

	/* F54_CONTROL 104 */
	if (query_22.has_ctrl104)
		ctrl_offset += F54_CONTROL_104_SIZE;

	/* F54_CONTROL 105 */
	if (query_22.has_ctrl105)
		ctrl_offset += F54_CONTROL_105_SIZE;

	/* F54_CONTROL 106 */
	if (query_25.has_ctrl106)
		ctrl_offset += F54_CONTROL_106_SIZE;

	/* F54_CONTROL 107 */
	if (query_25.has_ctrl107)
		ctrl_offset += F54_CONTROL_107_SIZE;

	/* F54_CONTROL 108 */
	if (query_25.has_ctrl108)
		ctrl_offset += F54_CONTROL_108_SIZE;

	/* F54_CONTROL 109 */
	if (query_25.has_ctrl109)
		ctrl_offset += F54_CONTROL_109_SIZE;

	/* F54_CONTROL 110 */
	if (query_27.has_ctrl110)
		ctrl_offset += F54_CONTROL_110_SIZE;

	/* F54_CONTROL 111 */
	if (query_27.has_ctrl111)
		ctrl_offset += F54_CONTROL_111_SIZE;

	/* F54_CONTROL 112 */
	if (query_27.has_ctrl112)
		ctrl_offset += F54_CONTROL_112_SIZE;

	/* F54_CONTROL 113 */
	if (query_27.has_ctrl113) {
		rmi4_data->f54_ctrl113_address = rmi4_data->f54_ctrl_base_addr + ctrl_offset;
		//ctrl_offset += F54_CONTROL_113_SIZE;
	}

	/* data 4 */
	if (f54_query.has_sense_frequency_control)
		data_offset += 1;

	/* data 5 reserved */

	/* data 6 */
	if (f54_query.has_interference_metric) {
		rmi4_data->f54_im_offset = data_offset + 1;
		data_offset += 2;
	}

	/* data 7.0 */
	if (f54_query.has_two_byte_report_rate |
			f54_query.has_one_byte_report_rate)
		data_offset += 1;

	/* data 7.1 */
	if (f54_query.has_two_byte_report_rate)
		data_offset += 1;

	/* data 8 */
	if (f54_query.has_variance_metric)
		data_offset += 2;

	/* data 9 */
	if (f54_query.has_multi_metric_state_machine)
		data_offset += 2;

	/* data 10 */
	if (f54_query.has_multi_metric_state_machine |
			f54_query.has_noise_state) {
		data_offset += 1;
		rmi4_data->f54_ns_offset = data_offset;
	}

	/* data 11 */
	if (f54_query.has_status)
		data_offset += 1;

	/* data 12 */
	if (f54_query.has_slew_metric)
		data_offset += 2;

	/* data 13 */
	if (f54_query.has_multi_metric_state_machine)
		data_offset += 2;

	/* data 14 */
	if (query_13.has_cidim) {
		data_offset += 1;
		rmi4_data->f54_cidim_offset = data_offset;
	}

	/* data 15 */
	if (query_13.has_rail_im)
		data_offset += 1;

	/* data 16 */
	if (query_13.has_noise_mitigation_enhancement)
		data_offset += 1;

	/* data 17 */
	if (query_16.has_data17) {
		data_offset += 1;
		rmi4_data->f54_freq_offset = data_offset;
	}

	if (rmi4_data->num_of_tx == 0 || rmi4_data->num_of_rx == 0) {
		dev_err(rmi4_data->pdev->dev.parent," %s num_of_tx(%d) or num_of_rx(%d) is zero\n", __func__, rmi4_data->num_of_tx, rmi4_data->num_of_rx);
		return -1;
	} else {
		rmi4_data->diag_command = F54_RAW_16BIT_IMAGE;

		rmi4_data->report_data = kzalloc(2 * rmi4_data->num_of_tx * rmi4_data->num_of_rx, GFP_KERNEL);
		if (rmi4_data->report_data == NULL) {
			dev_err(rmi4_data->pdev->dev.parent," %s report data init fail\n", __func__);
			return -1;
		}
		rmi4_data->report_data_32 = kzalloc(4 * (rmi4_data->num_of_tx + rmi4_data->num_of_rx), GFP_KERNEL);
		if (rmi4_data->report_data_32 == NULL) {
			dev_err(rmi4_data->pdev->dev.parent," %s report data 32 init fail\n", __func__);
			return -1;
		}
	}

	return 0;
}

static int synaptics_rmi4_f55_init(struct synaptics_rmi4_data *rmi4_data,
		struct synaptics_rmi4_fn *fhandler,
		struct synaptics_rmi4_fn_desc *fd,
		unsigned int intr_count,
		unsigned int page_number)
{
	struct synaptics_rmi4_f55_query f55_query;
	int retval;
	unsigned char offset = 0;
	fhandler->fn_number = fd->fn_number;
	fhandler->num_of_data_sources = fd->intr_src_count;
	fhandler->data = NULL;
	fhandler->extra = NULL;

	synaptics_rmi4_set_intr_mask(fhandler, fd, intr_count);

	rmi4_data->f55_query_base_addr =
		(fd->query_base_addr | (page_number << 8));
	rmi4_data->f55_ctrl_base_addr =
		(fd->ctrl_base_addr | (page_number << 8));
	rmi4_data->f55_data_base_addr =
		(fd->data_base_addr | (page_number << 8));
	rmi4_data->f55_cmd_base_addr =
		(fd->cmd_base_addr | (page_number << 8));

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f55_query_base_addr,
			f55_query.data,
			sizeof(f55_query.data));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read F55 query registers\n",
				__func__);
		return retval;
	}

	if (!f55_query.has_sensor_assignment)
		return 0;

	/* controls 0 1 2 */
	if (f55_query.has_sensor_assignment)
		offset += 3;

	/* control 3 */
	if (f55_query.has_edge_compensation) {
		rmi4_data->f55_ctrl03_offset = offset;
//		offset++;
	}

	return 0;
}
#endif

static void synaptics_rmi4_empty_fn_list(struct synaptics_rmi4_data *rmi4_data)
{
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_fn *fhandler_temp;
	struct synaptics_rmi4_device_info *rmi;

	rmi = &(rmi4_data->rmi4_mod_info);

	if (!list_empty(&rmi->support_fn_list)) {
		list_for_each_entry_safe(fhandler,
				fhandler_temp,
				&rmi->support_fn_list,
				link) {
			if (fhandler->fn_number == SYNAPTICS_RMI4_F1A) {
				synaptics_rmi4_f1a_kfree(fhandler);
			} else {
				kfree(fhandler->extra);
				kfree(fhandler->data);
			}
			list_del(&fhandler->link);
			kfree(fhandler);
		}
	}
	INIT_LIST_HEAD(&rmi->support_fn_list);

	return;
}

static int synaptics_rmi4_check_status(struct synaptics_rmi4_data *rmi4_data,
		bool *was_in_bl_mode)
{
	int retval;
	int timeout = CHECK_STATUS_TIMEOUT_MS;
	struct synaptics_rmi4_f01_device_status status;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_data_base_addr,
			status.data,
			sizeof(status.data));
	if (retval < 0)
		return retval;

	while (status.status_code == STATUS_CRC_IN_PROGRESS) {
		if (timeout > 0)
			msleep(20);
		else
			return -EINVAL;

		retval = synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f01_data_base_addr,
				status.data,
				sizeof(status.data));
		if (retval < 0)
			return retval;

		timeout -= 20;
	}

	if (timeout != CHECK_STATUS_TIMEOUT_MS)
		*was_in_bl_mode = true;

	if (status.flash_prog == 1) {
		rmi4_data->flash_prog_mode = true;
		pr_notice("%s: In flash prog mode, status = 0x%02x\n",
				__func__,
				status.status_code);
	} else {
		rmi4_data->flash_prog_mode = false;
	}

	return 0;
}

static void synaptics_rmi4_set_configured(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	unsigned char device_ctrl;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_ctrl_base_addr,
			&device_ctrl,
			sizeof(device_ctrl));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to set configured\n",
				__func__);
		return;
	}

	rmi4_data->no_sleep_setting = device_ctrl & NO_SLEEP_ON;
	device_ctrl |= CONFIGURED;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f01_ctrl_base_addr,
			&device_ctrl,
			sizeof(device_ctrl));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to set configured\n",
				__func__);
	}

	return;
}

static int synaptics_rmi4_alloc_fh(struct synaptics_rmi4_fn **fhandler,
		struct synaptics_rmi4_fn_desc *rmi_fd, int page_number)
{
	*fhandler = kzalloc(sizeof(**fhandler), GFP_KERNEL);
	if (!(*fhandler))
		return -ENOMEM;

	(*fhandler)->full_addr.data_base =
			(rmi_fd->data_base_addr |
			(page_number << 8));
	(*fhandler)->full_addr.ctrl_base =
			(rmi_fd->ctrl_base_addr |
			(page_number << 8));
	(*fhandler)->full_addr.cmd_base =
			(rmi_fd->cmd_base_addr |
			(page_number << 8));
	(*fhandler)->full_addr.query_base =
			(rmi_fd->query_base_addr |
			(page_number << 8));

	return 0;
}

static int synaptics_rmi4_query_device(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	unsigned char page_number;
	unsigned char intr_count;
	unsigned char *f01_query;
	unsigned short pdt_entry_addr;
	bool f01found;
	bool f35found;
	bool was_in_bl_mode;
	struct synaptics_rmi4_fn_desc rmi_fd;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_device_info *rmi;
#ifdef HTC_FEATURE
	unsigned char ii, config_id_size;
	uint8_t read_data[32]={0};
#endif

	pr_info("%s\n", __func__);
	rmi = &(rmi4_data->rmi4_mod_info);

rescan_pdt:
	f01found = false;
	f35found = false;
	was_in_bl_mode = false;
	intr_count = 0;
	INIT_LIST_HEAD(&rmi->support_fn_list);

	/* Scan the page description tables of the pages to service */
	for (page_number = 0; page_number < PAGES_TO_SERVICE; page_number++) {
		for (pdt_entry_addr = PDT_START; pdt_entry_addr > PDT_END;
				pdt_entry_addr -= PDT_ENTRY_SIZE) {
			pdt_entry_addr |= (page_number << 8);

			retval = synaptics_rmi4_reg_read(rmi4_data,
					pdt_entry_addr,
					(unsigned char *)&rmi_fd,
					sizeof(rmi_fd));
			if (retval < 0)
				return retval;

			pdt_entry_addr &= ~(MASK_8BIT << 8);

			fhandler = NULL;

			if (rmi_fd.fn_number == 0) {
				dev_dbg(rmi4_data->pdev->dev.parent,
						"%s: Reached end of PDT\n",
						__func__);
				break;
			}

			dev_dbg(rmi4_data->pdev->dev.parent,
					"%s: F%02x found (page %d)\n",
					__func__, rmi_fd.fn_number,
					page_number);

			switch (rmi_fd.fn_number) {
			case SYNAPTICS_RMI4_F01:
				if (rmi_fd.intr_src_count == 0)
					break;

				f01found = true;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}

				retval = synaptics_rmi4_f01_init(rmi4_data,
						fhandler, &rmi_fd, intr_count);
				if (retval < 0)
					return retval;

				retval = synaptics_rmi4_check_status(rmi4_data,
						&was_in_bl_mode);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to check status\n",
							__func__);
					return retval;
				}

				if (was_in_bl_mode) {
					kfree(fhandler);
					fhandler = NULL;
					goto rescan_pdt;
				}

				if (rmi4_data->flash_prog_mode)
					goto flash_prog_mode;

				break;
			case SYNAPTICS_RMI4_F11:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}

				retval = synaptics_rmi4_f11_init(rmi4_data,
						fhandler, &rmi_fd, intr_count);
				if (retval < 0)
					return retval;
				break;
			case SYNAPTICS_RMI4_F12:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}

				retval = synaptics_rmi4_f12_init(rmi4_data,
						fhandler, &rmi_fd, intr_count);
				if (retval < 0)
					return retval;
				break;
			case SYNAPTICS_RMI4_F1A:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}

				retval = synaptics_rmi4_f1a_init(rmi4_data,
						fhandler, &rmi_fd, intr_count);
				if (retval < 0) {
#ifdef IGNORE_FN_INIT_FAILURE
					kfree(fhandler);
					fhandler = NULL;
#else
					return retval;
#endif
				}
				break;
#ifdef USE_DATA_SERVER
			case SYNAPTICS_RMI4_F21:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}

				fhandler->fn_number = rmi_fd.fn_number;
				fhandler->num_of_data_sources =
						rmi_fd.intr_src_count;

				synaptics_rmi4_set_intr_mask(fhandler, &rmi_fd,
						intr_count);
				break;
#endif
#ifdef HTC_FEATURE
			case SYNAPTICS_RMI4_F34:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}
				retval = synaptics_rmi4_f34_init(rmi4_data,
						fhandler, &rmi_fd, intr_count, page_number);
				if (retval < 0)
					return retval;
				break;
			case SYNAPTICS_RMI4_F54:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}
				retval = synaptics_rmi4_f54_init(rmi4_data,
						fhandler, &rmi_fd, intr_count, page_number);
				if (retval < 0)
					return retval;
				break;
			case SYNAPTICS_RMI4_F55:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}
				retval = synaptics_rmi4_f55_init(rmi4_data,
						fhandler, &rmi_fd, intr_count, page_number);
				if (retval < 0)
					return retval;
				break;
#endif
			case SYNAPTICS_RMI4_F35:
				f35found = true;
				break;
#ifdef HTC_FEATURE
			case SYNAPTICS_RMI4_F51:
				if (rmi_fd.intr_src_count == 0)
					break;

				retval = synaptics_rmi4_alloc_fh(&fhandler,
						&rmi_fd, page_number);
				if (retval < 0) {
					dev_err(rmi4_data->pdev->dev.parent,
							"%s: Failed to alloc for F%d\n",
							__func__,
							rmi_fd.fn_number);
					return retval;
				}

				retval = synaptics_rmi4_f51_init(rmi4_data,
						fhandler, &rmi_fd, intr_count, page_number);
				if (retval < 0)
					return retval;
				break;
#else
#ifdef F51_DISCRETE_FORCE
			case SYNAPTICS_RMI4_F51:
				rmi4_data->f51_query_base_addr =
						rmi_fd.query_base_addr |
						(page_number << 8);
				break;
#endif
#endif
			}

			/* Accumulate the interrupt count */
			intr_count += rmi_fd.intr_src_count;

			if (fhandler && rmi_fd.intr_src_count) {
				list_add_tail(&fhandler->link,
						&rmi->support_fn_list);
			}
		}
	}

	if (!f01found) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to find F01\n",
				__func__);
		if (!f35found) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to find F35\n",
					__func__);
			return -EINVAL;
		} else {
			pr_notice("%s: In microbootloader mode\n",
					__func__);
			return 0;
		}
	}

flash_prog_mode:
	rmi4_data->num_of_intr_regs = (intr_count + 7) / 8;
	dev_dbg(rmi4_data->pdev->dev.parent,
			"%s: Number of interrupt registers = %d\n",
			__func__, rmi4_data->num_of_intr_regs);

	f01_query = kmalloc(F01_STD_QUERY_LEN, GFP_KERNEL);
	if (!f01_query) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to alloc mem for f01_query\n",
				__func__);
		return -ENOMEM;
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_query_base_addr,
			f01_query,
			F01_STD_QUERY_LEN);
	if (retval < 0) {
		kfree(f01_query);
		return retval;
	}

	/* RMI Version 4.0 currently supported */
	rmi->version_major = 4;
	rmi->version_minor = 0;

	rmi->manufacturer_id = f01_query[0];
	rmi->product_props = f01_query[1];
	rmi->product_info[0] = f01_query[2];
	rmi->product_info[1] = f01_query[3];
        retval = secure_memcpy(rmi->product_id_string,
                        sizeof(rmi->product_id_string),
                        &f01_query[11],
                        F01_STD_QUERY_LEN - 11,
                        PRODUCT_ID_SIZE);
        if (retval < 0) {
                dev_err(rmi4_data->pdev->dev.parent,
                                "%s: Failed to copy product ID string\n",
                                __func__);
        }

	kfree(f01_query);

	if (rmi->manufacturer_id != 1) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Non-Synaptics device found, manufacturer ID = %d\n",
				__func__, rmi->manufacturer_id);
	}

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_query_base_addr + F01_BUID_ID_OFFSET,
			rmi->build_id,
			sizeof(rmi->build_id));
	if (retval < 0)
		return retval;

	rmi4_data->firmware_id = (unsigned int)rmi->build_id[0] +
			(unsigned int)rmi->build_id[1] * 0x100 +
			(unsigned int)rmi->build_id[2] * 0x10000;

	memset(rmi4_data->intr_mask, 0x00, sizeof(rmi4_data->intr_mask));

#ifdef HTC_FEATURE
	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_query_base_addr + F01_CHIP_ID_OFFSET,
			rmi->package_id,
			sizeof(rmi->package_id));
	if (retval < 0)
		return retval;

	rmi4_data->chip_id = (unsigned int)rmi->package_id[0] +
			(unsigned int)rmi->package_id[1] * 0x100;

	dev_info(rmi4_data->pdev->dev.parent, "%s: chip_id:%d, firmware_id:%d\n",
				__func__, rmi4_data->chip_id, rmi4_data->firmware_id);

	//Config
	if (rmi4_data->bl_version == BL_V7 || rmi4_data->bl_version == BL_V8)
		config_id_size = V7_CONFIG_ID_SIZE;
	else
		config_id_size = V5V6_CONFIG_ID_SIZE;

	dev_dbg(rmi4_data->pdev->dev.parent,
			"%s: bl_version = %d, config_id_size = %d\n",
			__func__, rmi4_data->bl_version, config_id_size);

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f34_ctrl_base_addr,
			read_data,
			sizeof(uint8_t) * config_id_size);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent," %s err r:4\n",__func__);
		return retval;
	}

	memset(rmi4_data->config_id, 0, sizeof(rmi4_data->config_id));
	for (ii = 0; ii < config_id_size; ii++)
		snprintf(rmi4_data->config_id + 2 * ii, 3, "%02X", read_data[ii]);

	dev_info(rmi4_data->pdev->dev.parent,
			"%s: config_id: %s\n",
			__func__,
			rmi4_data->config_id);
#endif

	/*
	 * Map out the interrupt bit masks for the interrupt sources
	 * from the registered function handlers.
	 */
	if (!list_empty(&rmi->support_fn_list)) {
		list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
			if (fhandler->num_of_data_sources) {
				rmi4_data->intr_mask[fhandler->intr_reg_num] |=
						fhandler->intr_mask;
			}
		}
	}

	if (rmi4_data->f11_wakeup_gesture || rmi4_data->f12_wakeup_gesture)
		rmi4_data->enable_wakeup_gesture = WAKEUP_GESTURE;
	else
		rmi4_data->enable_wakeup_gesture = false;

	synaptics_rmi4_set_configured(rmi4_data);

	return 0;
}

static int synaptics_rmi4_gpio_setup(int gpio, bool config, int dir, int state)
{
	int retval = 0;
	unsigned char buf[16];

	if (config) {
		snprintf(buf, PAGE_SIZE, "dsx_gpio_%u\n", gpio);

		retval = gpio_request(gpio, buf);
		if (retval) {
			pr_err("%s: Failed to get gpio %d (code: %d)",
					__func__, gpio, retval);
			return retval;
		}

		if (dir == 0)
			retval = gpio_direction_input(gpio);
		else
			retval = gpio_direction_output(gpio, state);
		if (retval) {
			pr_err("%s: Failed to set gpio %d direction",
					__func__, gpio);
			return retval;
		}
	} else {
		gpio_free(gpio);
	}

	return retval;
}

static void synaptics_rmi4_set_params(struct synaptics_rmi4_data *rmi4_data)
{
	unsigned char ii;
	struct synaptics_rmi4_f1a_handle *f1a;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_device_info *rmi;

	rmi = &(rmi4_data->rmi4_mod_info);

	input_set_abs_params(rmi4_data->input_dev,
			ABS_MT_POSITION_X, 0,
			rmi4_data->sensor_max_x, 0, 0);
	input_set_abs_params(rmi4_data->input_dev,
			ABS_MT_POSITION_Y, 0,
			rmi4_data->sensor_max_y, 0, 0);
#ifdef REPORT_2D_W
	input_set_abs_params(rmi4_data->input_dev,
			ABS_MT_TOUCH_MAJOR, 0,
			rmi4_data->max_touch_width, 0, 0);
	input_set_abs_params(rmi4_data->input_dev,
			ABS_MT_TOUCH_MINOR, 0,
			rmi4_data->max_touch_width, 0, 0);
#endif

	rmi4_data->input_settings.sensor_max_x = rmi4_data->sensor_max_x;
	rmi4_data->input_settings.sensor_max_y = rmi4_data->sensor_max_y;
	rmi4_data->input_settings.max_touch_width = rmi4_data->max_touch_width;

#ifdef REPORT_2D_PRESSURE
	if (rmi4_data->report_pressure) {
		input_set_abs_params(rmi4_data->input_dev,
				ABS_MT_PRESSURE, rmi4_data->force_min,
				rmi4_data->force_max, 0, 0);

		rmi4_data->input_settings.force_min = rmi4_data->force_min;
		rmi4_data->input_settings.force_max = rmi4_data->force_max;
	}
#elif defined(F51_DISCRETE_FORCE)
	input_set_abs_params(rmi4_data->input_dev,
			ABS_MT_PRESSURE, 0,
			FORCE_LEVEL_MAX, 0, 0);
#endif

#ifdef TYPE_B_PROTOCOL
#ifdef KERNEL_ABOVE_3_6
	input_mt_init_slots(rmi4_data->input_dev,
			rmi4_data->num_of_fingers, INPUT_MT_DIRECT);
#else
	input_mt_init_slots(rmi4_data->input_dev,
			rmi4_data->num_of_fingers);
#endif
#endif

	rmi4_data->input_settings.num_of_fingers = rmi4_data->num_of_fingers;

	f1a = NULL;
	if (!list_empty(&rmi->support_fn_list)) {
		list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
			if (fhandler->fn_number == SYNAPTICS_RMI4_F1A)
				f1a = fhandler->data;
		}
	}

	if (f1a) {
		for (ii = 0; ii < f1a->valid_button_count; ii++) {
			set_bit(f1a->button_map[ii],
					rmi4_data->input_dev->keybit);
			input_set_capability(rmi4_data->input_dev,
					EV_KEY, f1a->button_map[ii]);
		}

		rmi4_data->input_settings.valid_button_count =
				f1a->valid_button_count;
	}

	if (vir_button_map->nbuttons) {
		for (ii = 0; ii < vir_button_map->nbuttons; ii++) {
			set_bit(vir_button_map->map[ii * 5],
					rmi4_data->input_dev->keybit);
			input_set_capability(rmi4_data->input_dev,
					EV_KEY, vir_button_map->map[ii * 5]);
		}
	}

	if (rmi4_data->f11_wakeup_gesture || rmi4_data->f12_wakeup_gesture) {
		set_bit(KEY_WAKEUP, rmi4_data->input_dev->keybit);
		input_set_capability(rmi4_data->input_dev, EV_KEY, KEY_WAKEUP);
	}

	return;
}

static int synaptics_rmi4_set_input_dev(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	const struct synaptics_dsx_board_data *bdata =
				rmi4_data->hw_if->board_data;

	rmi4_data->input_dev = input_allocate_device();
	if (rmi4_data->input_dev == NULL) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to allocate input device\n",
				__func__);
		retval = -ENOMEM;
		goto err_input_device;
	}

	retval = synaptics_rmi4_query_device(rmi4_data);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to query device\n",
				__func__);
		goto err_query_device;
	}

	rmi4_data->input_dev->name = PLATFORM_DRIVER_NAME;
	rmi4_data->input_dev->phys = INPUT_PHYS_NAME;
	rmi4_data->input_dev->id.product = SYNAPTICS_DSX_DRIVER_PRODUCT;
	rmi4_data->input_dev->id.version = SYNAPTICS_DSX_DRIVER_VERSION;
	rmi4_data->input_dev->dev.parent = rmi4_data->pdev->dev.parent;
	input_set_drvdata(rmi4_data->input_dev, rmi4_data);

	set_bit(EV_SYN, rmi4_data->input_dev->evbit);
	set_bit(EV_KEY, rmi4_data->input_dev->evbit);
	set_bit(EV_ABS, rmi4_data->input_dev->evbit);
	set_bit(BTN_TOUCH, rmi4_data->input_dev->keybit);
	set_bit(BTN_TOOL_FINGER, rmi4_data->input_dev->keybit);
#ifdef INPUT_PROP_DIRECT
	set_bit(INPUT_PROP_DIRECT, rmi4_data->input_dev->propbit);
#endif

	if (bdata->max_y_for_2d >= 0)
		rmi4_data->sensor_max_y = bdata->max_y_for_2d;

	synaptics_rmi4_set_params(rmi4_data);

	retval = input_register_device(rmi4_data->input_dev);
	if (retval) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to register input device\n",
				__func__);
		goto err_register_input;
	}

	rmi4_data->input_settings.stylus_enable = rmi4_data->stylus_enable;
	rmi4_data->input_settings.eraser_enable = rmi4_data->eraser_enable;

	if (!rmi4_data->stylus_enable)
		return 0;

	rmi4_data->stylus_dev = input_allocate_device();
	if (rmi4_data->stylus_dev == NULL) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to allocate stylus device\n",
				__func__);
		retval = -ENOMEM;
		goto err_stylus_device;
	}

	rmi4_data->stylus_dev->name = STYLUS_DRIVER_NAME;
	rmi4_data->stylus_dev->phys = STYLUS_PHYS_NAME;
	rmi4_data->stylus_dev->id.product = SYNAPTICS_DSX_DRIVER_PRODUCT;
	rmi4_data->stylus_dev->id.version = SYNAPTICS_DSX_DRIVER_VERSION;
	rmi4_data->stylus_dev->dev.parent = rmi4_data->pdev->dev.parent;
	input_set_drvdata(rmi4_data->stylus_dev, rmi4_data);

	set_bit(EV_KEY, rmi4_data->stylus_dev->evbit);
	set_bit(EV_ABS, rmi4_data->stylus_dev->evbit);
	set_bit(BTN_TOUCH, rmi4_data->stylus_dev->keybit);
	set_bit(BTN_TOOL_PEN, rmi4_data->stylus_dev->keybit);
	if (rmi4_data->eraser_enable)
		set_bit(BTN_TOOL_RUBBER, rmi4_data->stylus_dev->keybit);
#ifdef INPUT_PROP_DIRECT
	set_bit(INPUT_PROP_DIRECT, rmi4_data->stylus_dev->propbit);
#endif

	input_set_abs_params(rmi4_data->stylus_dev, ABS_X, 0,
			rmi4_data->sensor_max_x, 0, 0);
	input_set_abs_params(rmi4_data->stylus_dev, ABS_Y, 0,
			rmi4_data->sensor_max_y, 0, 0);

	retval = input_register_device(rmi4_data->stylus_dev);
	if (retval) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to register stylus device\n",
				__func__);
		goto err_register_stylus;
	}

	return 0;

err_register_stylus:
	rmi4_data->stylus_dev = NULL;

err_stylus_device:
	input_unregister_device(rmi4_data->input_dev);
	rmi4_data->input_dev = NULL;

err_register_input:
err_query_device:
#ifdef HTC_FEATURE
	if (rmi4_data->report_data != NULL)
		kfree(rmi4_data->report_data);
	if (rmi4_data->report_data_32 != NULL)
		kfree(rmi4_data->report_data_32);
#endif
	synaptics_rmi4_empty_fn_list(rmi4_data);
	input_free_device(rmi4_data->input_dev);

err_input_device:
	return retval;
}

static int synaptics_rmi4_set_gpio(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	retval = synaptics_rmi4_gpio_setup(
			bdata->irq_gpio,
			true, 0, 0);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to configure attention GPIO\n",
				__func__);
		goto err_gpio_irq;
	}

	if (bdata->power_gpio >= 0) {
		retval = synaptics_rmi4_gpio_setup(
				bdata->power_gpio,
				true, 1, !bdata->power_on_state);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to configure power GPIO\n",
					__func__);
			goto err_gpio_power;
		}
	}

	if (bdata->reset_gpio >= 0) {
		retval = synaptics_rmi4_gpio_setup(
				bdata->reset_gpio,
				true, 1, !bdata->reset_on_state);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to configure reset GPIO\n",
					__func__);
			goto err_gpio_reset;
		}
	}

	if (bdata->power_gpio >= 0) {
		gpio_set_value(bdata->power_gpio, bdata->power_on_state);
		msleep(bdata->power_delay_ms);
	}

	if (bdata->reset_gpio >= 0) {
		gpio_set_value(bdata->reset_gpio, bdata->reset_on_state);
		msleep(bdata->reset_active_ms);
		gpio_set_value(bdata->reset_gpio, !bdata->reset_on_state);
		msleep(bdata->reset_delay_ms);
	}

#ifdef HTC_FEATURE
	if (bdata->switch_gpio >= 0) {
		retval = synaptics_rmi4_gpio_setup(
				bdata->switch_gpio,
				true, 1, !bdata->switch_hub_state);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to configure switch GPIO\n",
					__func__);
			goto err_gpio_switch;
		}
		rmi4_data->i2c_to_mcu = SWITCH_TO_CPU;
	}
#endif

	return 0;

#ifdef HTC_FEATURE
err_gpio_switch:
	if (bdata->reset_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->reset_gpio, false, 0, 0);
#endif

err_gpio_reset:
	if (bdata->power_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->power_gpio, false, 0, 0);

err_gpio_power:
	synaptics_rmi4_gpio_setup(bdata->irq_gpio, false, 0, 0);

err_gpio_irq:
	return retval;
}

static int synaptics_rmi4_get_reg(struct synaptics_rmi4_data *rmi4_data,
		bool get)
{
	int retval;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (!get) {
		retval = 0;
		goto regulator_put;
	}

	if ((bdata->pwr_reg_name != NULL) && (*bdata->pwr_reg_name != 0)) {
		rmi4_data->pwr_reg = regulator_get(rmi4_data->pdev->dev.parent,
				bdata->pwr_reg_name);
		if (IS_ERR(rmi4_data->pwr_reg)) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to get power regulator\n",
					__func__);
			retval = PTR_ERR(rmi4_data->pwr_reg);
			goto regulator_put;
		}
	}

	if ((bdata->bus_reg_name != NULL) && (*bdata->bus_reg_name != 0)) {
		rmi4_data->bus_reg = regulator_get(rmi4_data->pdev->dev.parent,
				bdata->bus_reg_name);
		if (IS_ERR(rmi4_data->bus_reg)) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to get bus pullup regulator\n",
					__func__);
			retval = PTR_ERR(rmi4_data->bus_reg);
			goto regulator_put;
		}
	}

	return 0;

regulator_put:
	if (rmi4_data->pwr_reg) {
		regulator_put(rmi4_data->pwr_reg);
		rmi4_data->pwr_reg = NULL;
	}

	if (rmi4_data->bus_reg) {
		regulator_put(rmi4_data->bus_reg);
		rmi4_data->bus_reg = NULL;
	}

	return retval;
}

static int synaptics_rmi4_enable_reg(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	int retval;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (!enable) {
		retval = 0;
		goto disable_pwr_reg;
	}

	if (rmi4_data->bus_reg) {
		retval = regulator_enable(rmi4_data->bus_reg);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to enable bus pullup regulator\n",
					__func__);
			goto exit;
		}
	}

	if (rmi4_data->pwr_reg) {
		retval = regulator_enable(rmi4_data->pwr_reg);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to enable power regulator\n",
					__func__);
			goto disable_bus_reg;
		}
		msleep(bdata->power_delay_ms);
	}

	return 0;

disable_pwr_reg:
	if (rmi4_data->pwr_reg)
		regulator_disable(rmi4_data->pwr_reg);

disable_bus_reg:
	if (rmi4_data->bus_reg)
		regulator_disable(rmi4_data->bus_reg);

exit:
	return retval;
}

static int synaptics_rmi4_free_fingers(struct synaptics_rmi4_data *rmi4_data)
{
	unsigned char ii;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	mutex_lock(&(rmi4_data->rmi4_report_mutex));

#ifdef TYPE_B_PROTOCOL
	for (ii = 0; ii < rmi4_data->num_of_fingers; ii++) {
		input_mt_slot(rmi4_data->input_dev, ii);
		input_mt_report_slot_state(rmi4_data->input_dev,
				MT_TOOL_FINGER, 0);
#ifdef HTC_FEATURE
		if (rmi4_data->debug_mask & (TOUCH_DOWN_UP_LOG | TOUCH_KPI_LOG | TOUCH_BREAKDOWN_TIME))
			rmi4_data->report_points[ii].state = 0;
#endif
	}
#endif
	input_report_key(rmi4_data->input_dev,
			BTN_TOUCH, 0);
	input_report_key(rmi4_data->input_dev,
			BTN_TOOL_FINGER, 0);
#ifndef TYPE_B_PROTOCOL
	input_mt_sync(rmi4_data->input_dev);
#endif
	input_sync(rmi4_data->input_dev);

#ifdef HTC_FEATURE
	if (bdata->support_eg_blk && rmi4_data->touch_detect == true) {
		rmi4_data->touch_detect = false;
		if (rmi4_data->debug_mask & TOUCH_DOWN_UP_LOG) {
			pr_info("%s: set touch_detect %s\n", __func__, rmi4_data->touch_detect ? "enabled" : "disabled");
		}
		report_touch_detect(rmi4_data);
	}
#endif

	if (rmi4_data->stylus_enable) {
		input_report_key(rmi4_data->stylus_dev,
				BTN_TOUCH, 0);
		input_report_key(rmi4_data->stylus_dev,
				BTN_TOOL_PEN, 0);
		if (rmi4_data->eraser_enable) {
			input_report_key(rmi4_data->stylus_dev,
					BTN_TOOL_RUBBER, 0);
		}
		input_sync(rmi4_data->stylus_dev);
	}

	mutex_unlock(&(rmi4_data->rmi4_report_mutex));

	rmi4_data->fingers_on_2d = false;

	return 0;
}

#ifdef HTC_FEATURE
static int synaptics_rmi4_hw_reset(struct synaptics_rmi4_data *rmi4_data)
{
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;
	int retval = 0;

	pr_info("%s", __func__);

	mutex_lock(&(rmi4_data->rmi4_reset_mutex));

	if (bdata->reset_gpio >= 0) {
		gpio_direction_output(bdata->reset_gpio, bdata->reset_on_state);
		msleep(bdata->reset_active_ms);
		gpio_set_value(bdata->reset_gpio, !bdata->reset_on_state);
	} else {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Not support HW reset\n", __func__);
		retval = -ENODEV;
		goto unlock;
	}

	if (rmi4_data->hw_if->ui_hw_init) {
		retval = rmi4_data->hw_if->ui_hw_init(rmi4_data);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to issue reset command, error = %d\n", __func__, retval);
		}
	}

#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
	rmi4_data->skip_esd = true;
#endif

unlock:
	mutex_unlock(&(rmi4_data->rmi4_reset_mutex));
	return retval;
}
#endif

static int synaptics_rmi4_sw_reset(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	unsigned char command = 0x01;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f01_cmd_base_addr,
			&command,
			sizeof(command));
	if (retval < 0)
		return retval;

	msleep(rmi4_data->hw_if->board_data->reset_delay_ms);

	if (rmi4_data->hw_if->ui_hw_init) {
		retval = rmi4_data->hw_if->ui_hw_init(rmi4_data);
		if (retval < 0)
			return retval;
	}

	return 0;
}

static int synaptics_rmi4_do_rebuild(struct synaptics_rmi4_data *rmi4_data)
{
	struct synaptics_rmi4_input_settings *settings;

	settings = &(rmi4_data->input_settings);

	if (settings->num_of_fingers != rmi4_data->num_of_fingers)
		return 1;

	if (settings->valid_button_count != rmi4_data->valid_button_count)
		return 1;

	if (settings->max_touch_width != rmi4_data->max_touch_width)
		return 1;

	if (settings->sensor_max_x != rmi4_data->sensor_max_x)
		return 1;

	if (settings->sensor_max_y != rmi4_data->sensor_max_y)
		return 1;

	if (settings->force_min != rmi4_data->force_min)
		return 1;

	if (settings->force_max != rmi4_data->force_max)
		return 1;

	if (settings->stylus_enable != rmi4_data->stylus_enable)
		return 1;

	if (settings->eraser_enable != rmi4_data->eraser_enable)
		return 1;

	return 0;
}

static void synaptics_rmi4_rebuild_work(struct work_struct *work)
{
	int retval;
	unsigned char attr_count;
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct delayed_work *delayed_work =
			container_of(work, struct delayed_work, work);
	struct synaptics_rmi4_data *rmi4_data =
			container_of(delayed_work, struct synaptics_rmi4_data,
			rb_work);

	mutex_lock(&(rmi4_data->rmi4_reset_mutex));

	mutex_lock(&exp_data.mutex);

	synaptics_rmi4_irq_enable(rmi4_data, false, false);

	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->remove != NULL)
				exp_fhandler->exp_fn->remove(rmi4_data);
	}

	for (attr_count = 0; attr_count < ARRAY_SIZE(attrs); attr_count++) {
		sysfs_remove_file(&rmi4_data->input_dev->dev.kobj,
				&attrs[attr_count].attr);
	}

	synaptics_rmi4_free_fingers(rmi4_data);
	synaptics_rmi4_empty_fn_list(rmi4_data);
	input_unregister_device(rmi4_data->input_dev);
	rmi4_data->input_dev = NULL;
	if (rmi4_data->stylus_enable) {
		input_unregister_device(rmi4_data->stylus_dev);
		rmi4_data->stylus_dev = NULL;
	}

	retval = synaptics_rmi4_set_input_dev(rmi4_data);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to set up input device\n",
				__func__);
		goto exit;
	}

	for (attr_count = 0; attr_count < ARRAY_SIZE(attrs); attr_count++) {
		retval = sysfs_create_file(&rmi4_data->input_dev->dev.kobj,
				&attrs[attr_count].attr);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to create sysfs attributes\n",
					__func__);
			goto exit;
		}
	}

	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->init != NULL)
				exp_fhandler->exp_fn->init(rmi4_data);
	}

	retval = 0;

exit:
	synaptics_rmi4_irq_enable(rmi4_data, true, false);

	mutex_unlock(&exp_data.mutex);

	mutex_unlock(&(rmi4_data->rmi4_reset_mutex));

	return;
}

static int synaptics_rmi4_reinit_device(struct synaptics_rmi4_data *rmi4_data)
{
	int retval;
	struct synaptics_rmi4_fn *fhandler;
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_device_info *rmi;

	pr_info("%s", __func__);
	rmi = &(rmi4_data->rmi4_mod_info);

	mutex_lock(&(rmi4_data->rmi4_reset_mutex));

	synaptics_rmi4_free_fingers(rmi4_data);

	if (!list_empty(&rmi->support_fn_list)) {
		list_for_each_entry(fhandler, &rmi->support_fn_list, link) {
			if (fhandler->fn_number == SYNAPTICS_RMI4_F12) {
				synaptics_rmi4_f12_set_enables(rmi4_data, 0);
				break;
			}
		}
	}

	retval = synaptics_rmi4_int_enable(rmi4_data, true);
	if (retval < 0)
		goto exit;

	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->reinit != NULL)
				exp_fhandler->exp_fn->reinit(rmi4_data);
	}
	mutex_unlock(&exp_data.mutex);

	synaptics_rmi4_set_configured(rmi4_data);

#ifdef HTC_FEATURE
	synaptics_rmi4_set_mode_reg(rmi4_data);
#endif

	retval = 0;

exit:
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
	rmi4_data->skip_esd = false;
#endif
	mutex_unlock(&(rmi4_data->rmi4_reset_mutex));
	return retval;
}

static int synaptics_rmi4_reset_device(struct synaptics_rmi4_data *rmi4_data,
		bool rebuild)
{
	int retval;
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;

	pr_info("%s", __func__);
	mutex_lock(&(rmi4_data->rmi4_reset_mutex));

	synaptics_rmi4_irq_enable(rmi4_data, false, false);

	retval = synaptics_rmi4_sw_reset(rmi4_data);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to issue reset command\n",
				__func__);
		goto exit;
	}

	synaptics_rmi4_free_fingers(rmi4_data);

	synaptics_rmi4_empty_fn_list(rmi4_data);

	retval = synaptics_rmi4_query_device(rmi4_data);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to query device\n",
				__func__);
		goto exit;
	}

	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->reset != NULL)
				exp_fhandler->exp_fn->reset(rmi4_data);
	}
	mutex_unlock(&exp_data.mutex);

#ifdef HTC_FEATURE
	synaptics_rmi4_set_mode_reg(rmi4_data);
#endif

	retval = 0;

exit:
	synaptics_rmi4_irq_enable(rmi4_data, true, false);

	mutex_unlock(&(rmi4_data->rmi4_reset_mutex));

	if (rebuild && synaptics_rmi4_do_rebuild(rmi4_data)) {
		queue_delayed_work(rmi4_data->rb_workqueue,
				&rmi4_data->rb_work,
				msecs_to_jiffies(REBUILD_WORK_DELAY_MS));
	}

	return retval;
}

#ifdef FB_READY_RESET
static void synaptics_rmi4_reset_work(struct work_struct *work)
{
	int retval;
	unsigned int timeout;
	struct synaptics_rmi4_data *rmi4_data =
			container_of(work, struct synaptics_rmi4_data,
			reset_work);

	timeout = FB_READY_TIMEOUT_S * 1000 / FB_READY_WAIT_MS + 1;

	while (!rmi4_data->fb_ready) {
		msleep(FB_READY_WAIT_MS);
		timeout--;
		if (timeout == 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Timed out waiting for FB ready\n",
					__func__);
			return;
		}
	}

	mutex_lock(&rmi4_data->rmi4_exp_init_mutex);

	retval = synaptics_rmi4_reset_device(rmi4_data, false);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to issue reset command\n",
				__func__);
	}

	mutex_unlock(&rmi4_data->rmi4_exp_init_mutex);

	return;
}
#endif

static void synaptics_rmi4_sleep_enable(struct synaptics_rmi4_data *rmi4_data,
		bool enable)
{
	int retval;
	unsigned char device_ctrl;
	unsigned char no_sleep_setting = rmi4_data->no_sleep_setting;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			rmi4_data->f01_ctrl_base_addr,
			&device_ctrl,
			sizeof(device_ctrl));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to read device control\n",
				__func__);
		return;
	}

	device_ctrl = device_ctrl & ~MASK_3BIT;
	if (enable)
		device_ctrl = device_ctrl | SENSOR_SLEEP;
	else
		device_ctrl = device_ctrl | no_sleep_setting | NORMAL_OPERATION;

	retval = synaptics_rmi4_reg_write(rmi4_data,
			rmi4_data->f01_ctrl_base_addr,
			&device_ctrl,
			sizeof(device_ctrl));
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to write device control\n",
				__func__);
		return;
	}

	rmi4_data->sensor_sleep = enable;

	return;
}

static void synaptics_rmi4_exp_fn_work(struct work_struct *work)
{
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_exp_fhandler *exp_fhandler_temp;
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;

	mutex_lock(&rmi4_data->rmi4_exp_init_mutex);
	mutex_lock(&rmi4_data->rmi4_reset_mutex);
	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry_safe(exp_fhandler,
				exp_fhandler_temp,
				&exp_data.list,
				link) {
			if ((exp_fhandler->exp_fn->init != NULL) &&
					exp_fhandler->insert) {
				exp_fhandler->exp_fn->init(rmi4_data);
				exp_fhandler->insert = false;
			} else if ((exp_fhandler->exp_fn->remove != NULL) &&
					exp_fhandler->remove) {
				exp_fhandler->exp_fn->remove(rmi4_data);
				list_del(&exp_fhandler->link);
				kfree(exp_fhandler);
			}
		}
	}
	mutex_unlock(&exp_data.mutex);
	mutex_unlock(&rmi4_data->rmi4_reset_mutex);
	mutex_unlock(&rmi4_data->rmi4_exp_init_mutex);

	return;
}

void synaptics_rmi4_new_function(struct synaptics_rmi4_exp_fn *exp_fn,
		bool insert)
{
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;

	if (!exp_data.initialized) {
		mutex_init(&exp_data.mutex);
		INIT_LIST_HEAD(&exp_data.list);
		exp_data.initialized = true;
	}

	mutex_lock(&exp_data.mutex);
	if (insert) {
		exp_fhandler = kzalloc(sizeof(*exp_fhandler), GFP_KERNEL);
		if (!exp_fhandler) {
			pr_err("%s: Failed to alloc mem for expansion function\n",
					__func__);
			goto exit;
		}
		exp_fhandler->exp_fn = exp_fn;
		exp_fhandler->insert = true;
		exp_fhandler->remove = false;
		list_add_tail(&exp_fhandler->link, &exp_data.list);
	} else if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link) {
			if (exp_fhandler->exp_fn->fn_type == exp_fn->fn_type) {
				exp_fhandler->insert = false;
				exp_fhandler->remove = true;
				goto exit;
			}
		}
	}

exit:
	mutex_unlock(&exp_data.mutex);

	if (exp_data.queue_work) {
		queue_delayed_work(exp_data.workqueue,
				&exp_data.work,
				msecs_to_jiffies(EXP_FN_WORK_DELAY_MS));
	}

	return;
}
EXPORT_SYMBOL(synaptics_rmi4_new_function);

#ifdef HTC_FEATURE
static int check_chip_exist(struct synaptics_rmi4_data *rmi4_data)
{
	unsigned char data;
	int retval;

	retval = synaptics_rmi4_reg_read(rmi4_data,
			(PDT_PROPS-1),
			&data,
			sizeof(data));
	if (retval > 0 && (data == SYNAPTICS_RMI4_F34)) {
		dev_info(rmi4_data->pdev->dev.parent, "%s: Synaptics chip exist\n", __func__);
		return 0;
	}

	return -1;
}
#endif

static int synaptics_rmi4_probe(struct platform_device *pdev)
{
	int retval;
	int attr_count;
	struct synaptics_rmi4_data *rmi4_data;
	const struct synaptics_dsx_hw_interface *hw_if;
	const struct synaptics_dsx_board_data *bdata;

#ifdef HTC_FEATURE
	static char *htc_bootmode = NULL;
	htc_bootmode = htc_get_bootmode();
	pr_info("%s: htc_bootmode = %s", __func__, htc_bootmode);
	if ((strcmp(htc_bootmode, "offmode_charging") == 0) ||
			(strcmp(htc_bootmode, "recovery") == 0)) {
		pr_info("%s: --skip--", __func__);
		return -ENODEV;
	}
#endif

	hw_if = pdev->dev.platform_data;
	if (!hw_if) {
		dev_err(&pdev->dev,
				"%s: No hardware interface found\n",
				__func__);
		return -EINVAL;
	}

	bdata = hw_if->board_data;
	if (!bdata) {
		dev_err(&pdev->dev,
				"%s: No board data found\n",
				__func__);
		return -EINVAL;
	}

	rmi4_data = kzalloc(sizeof(*rmi4_data), GFP_KERNEL);
	if (!rmi4_data) {
		dev_err(&pdev->dev,
				"%s: Failed to alloc mem for rmi4_data\n",
				__func__);
		return -ENOMEM;
	}

	rmi4_data->pdev = pdev;
	rmi4_data->current_page = MASK_8BIT;
	rmi4_data->hw_if = hw_if;
	rmi4_data->suspend = false;
	rmi4_data->irq_enabled = false;
	rmi4_data->fingers_on_2d = false;
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_DSX_ESD_DETECTION
	rmi4_data->skip_esd = false;
#endif
#ifdef HTC_FEATURE
	rmi4_data->aod_mode = false;
#endif

	rmi4_data->reset_device = synaptics_rmi4_reset_device;
	rmi4_data->irq_enable = synaptics_rmi4_irq_enable;
	rmi4_data->sleep_enable = synaptics_rmi4_sleep_enable;
	rmi4_data->report_touch = synaptics_rmi4_report_touch;

	mutex_init(&(rmi4_data->rmi4_reset_mutex));
	mutex_init(&(rmi4_data->rmi4_report_mutex));
	mutex_init(&(rmi4_data->rmi4_io_ctrl_mutex));
	mutex_init(&(rmi4_data->rmi4_exp_init_mutex));
	mutex_init(&(rmi4_data->rmi4_notifier_mutex));
	mutex_init(&(rmi4_data->rmi4_irq_enable_mutex));

	platform_set_drvdata(pdev, rmi4_data);

	vir_button_map = bdata->vir_button_map;

	retval = synaptics_rmi4_get_reg(rmi4_data, true);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to get regulators\n",
				__func__);
		goto err_get_reg;
	}

	retval = synaptics_rmi4_enable_reg(rmi4_data, true);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to enable regulators\n",
				__func__);
		goto err_enable_reg;
	}

	retval = synaptics_rmi4_set_gpio(rmi4_data);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to set up GPIO's\n",
				__func__);
		goto err_set_gpio;
	}

	if (hw_if->ui_hw_init) {
		retval = hw_if->ui_hw_init(rmi4_data);
		if (retval < 0) {
			dev_err(&pdev->dev,
					"%s: Failed to initialize hardware interface\n",
					__func__);
			goto err_ui_hw_init;
		}
	}

#ifdef HTC_FEATURE
	if (check_chip_exist(rmi4_data) < 0) {
		dev_info(&pdev->dev, "%s: No Synaptics chip\n", __func__);
		retval = -EPROBE_DEFER;
		goto err_set_input_dev;
	}
#endif

	retval = synaptics_rmi4_set_input_dev(rmi4_data);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to set up input device\n",
				__func__);
		goto err_set_input_dev;
	}

#ifdef CONFIG_FB
	rmi4_data->fb_notifier.notifier_call = synaptics_rmi4_fb_notifier_cb;
	retval = fb_register_client(&rmi4_data->fb_notifier);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to register fb notifier client\n",
				__func__);
	}
#endif

#ifdef HTC_FEATURE
#ifdef CONFIG_TOUCHSCREEN_USE_DSI_NOTIFY
	dsi_register_notifier(&dsi_event_notifier);
#endif
#ifdef CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY
	rmi4_data->drm_notifier.notifier_call = synaptics_rmi4_drm_notifier_cb;
	retval = msm_drm_register_client(&rmi4_data->drm_notifier);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to register drm notifier client\n",
				__func__);
	}
#endif
#endif

#ifdef USE_EARLYSUSPEND
	rmi4_data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	rmi4_data->early_suspend.suspend = synaptics_rmi4_early_suspend;
	rmi4_data->early_suspend.resume = synaptics_rmi4_late_resume;
	register_early_suspend(&rmi4_data->early_suspend);
#endif

	if (!exp_data.initialized) {
		mutex_init(&exp_data.mutex);
		INIT_LIST_HEAD(&exp_data.list);
		exp_data.initialized = true;
	}

	rmi4_data->irq = gpio_to_irq(bdata->irq_gpio);

#ifdef HTC_FEATURE
	retval = request_threaded_irq(rmi4_data->irq, NULL,
			synaptics_rmi4_irq, bdata->irq_flags,
			PLATFORM_DRIVER_NAME, rmi4_data);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to create irq thread\n",
				__func__);
		goto err_enable_irq;
	}

	rmi4_data->irq_enabled = true;
#endif

	retval = synaptics_rmi4_irq_enable(rmi4_data, true, false);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to enable attention interrupt\n",
				__func__);
		goto err_enable_irq;
	}

	if (vir_button_map->nbuttons) {
		rmi4_data->board_prop_dir = kobject_create_and_add(
				"board_properties", NULL);
		if (!rmi4_data->board_prop_dir) {
			dev_err(&pdev->dev,
					"%s: Failed to create board_properties directory\n",
					__func__);
			goto err_virtual_buttons;
		} else {
			retval = sysfs_create_file(rmi4_data->board_prop_dir,
					&virtual_key_map_attr.attr);
			if (retval < 0) {
				dev_err(&pdev->dev,
						"%s: Failed to create virtual key map file\n",
						__func__);
				goto err_virtual_buttons;
			}
		}
	}

#ifdef HTC_FEATURE
	retval = synaptics_rmi4_sysfs_init(rmi4_data, true);
	if (retval < 0) {
		dev_err(&pdev->dev,
				"%s: Failed to create sysfs\n", __func__);
		goto err_sysfs_init;
	}
#endif

	for (attr_count = 0; attr_count < ARRAY_SIZE(attrs); attr_count++) {
		retval = sysfs_create_file(&rmi4_data->input_dev->dev.kobj,
				&attrs[attr_count].attr);
		if (retval < 0) {
			dev_err(&pdev->dev,
					"%s: Failed to create sysfs attributes\n",
					__func__);
			goto err_sysfs;
		}
	}

#ifdef USE_DATA_SERVER
	memset(&interrupt_signal, 0, sizeof(interrupt_signal));
	interrupt_signal.si_signo = SIGIO;
	interrupt_signal.si_code = SI_USER;
#endif

#ifdef HTC_FEATURE
	if (tamper_flag == 0) {
		rmi4_data->debug_mask = TOUCH_DOWN_UP_LOG;
		pr_info(" Debug level=0x%08X\n", rmi4_data->debug_mask);
		rmi4_data->diag_command = F54_RAW_16BIT_IMAGE;
	} else
		rmi4_data->debug_mask = 0;

	rmi4_data->noise_state.im_m = 0;
	rmi4_data->noise_state.cidim_m = 0;

	init_waitqueue_head(&syn_data_ready_wq);
#endif

	rmi4_data->rb_workqueue =
			create_singlethread_workqueue("dsx_rebuild_workqueue");
	INIT_DELAYED_WORK(&rmi4_data->rb_work, synaptics_rmi4_rebuild_work);

	exp_data.workqueue = create_singlethread_workqueue("dsx_exp_workqueue");
	INIT_DELAYED_WORK(&exp_data.work, synaptics_rmi4_exp_fn_work);
	exp_data.rmi4_data = rmi4_data;
	exp_data.queue_work = true;
	queue_delayed_work(exp_data.workqueue,
			&exp_data.work,
			0);

#ifdef FB_READY_RESET
	rmi4_data->reset_workqueue =
			create_singlethread_workqueue("dsx_reset_workqueue");
	INIT_WORK(&rmi4_data->reset_work, synaptics_rmi4_reset_work);
	queue_work(rmi4_data->reset_workqueue, &rmi4_data->reset_work);
#endif

        synaptics_secure_touch_init(rmi4_data);
        synaptics_secure_touch_stop(rmi4_data, 1);
        return retval;

err_sysfs:
	for (attr_count--; attr_count >= 0; attr_count--) {
		sysfs_remove_file(&rmi4_data->input_dev->dev.kobj,
				&attrs[attr_count].attr);
	}

#ifdef HTC_FEATURE
err_sysfs_init:
	synaptics_rmi4_sysfs_init(rmi4_data, false);
#endif

err_virtual_buttons:
	if (rmi4_data->board_prop_dir) {
		sysfs_remove_file(rmi4_data->board_prop_dir,
				&virtual_key_map_attr.attr);
		kobject_put(rmi4_data->board_prop_dir);
	}

	synaptics_rmi4_irq_enable(rmi4_data, false, false);

err_enable_irq:
#ifdef CONFIG_FB
	fb_unregister_client(&rmi4_data->fb_notifier);
#endif
#ifdef HTC_FEATURE
#ifdef CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY
	msm_drm_unregister_client(&rmi4_data->drm_notifier);
#endif
#endif

#ifdef USE_EARLYSUSPEND
	unregister_early_suspend(&rmi4_data->early_suspend);
#endif

	synaptics_rmi4_empty_fn_list(rmi4_data);
	input_unregister_device(rmi4_data->input_dev);
	rmi4_data->input_dev = NULL;
	if (rmi4_data->stylus_enable) {
		input_unregister_device(rmi4_data->stylus_dev);
		rmi4_data->stylus_dev = NULL;
	}

err_set_input_dev:
	synaptics_rmi4_gpio_setup(bdata->irq_gpio, false, 0, 0);

#ifdef HTC_FEATURE
	if (bdata->switch_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->switch_gpio, false, 0, 0);
#endif

	if (bdata->reset_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->reset_gpio, false, 0, 0);

	if (bdata->power_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->power_gpio, false, 0, 0);

err_ui_hw_init:
err_set_gpio:
	synaptics_rmi4_enable_reg(rmi4_data, false);

err_enable_reg:
	synaptics_rmi4_get_reg(rmi4_data, false);

err_get_reg:
	kfree(rmi4_data);

	return retval;
}

static int synaptics_rmi4_remove(struct platform_device *pdev)
{
	unsigned char attr_count;
	struct synaptics_rmi4_data *rmi4_data = platform_get_drvdata(pdev);
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

#ifdef FB_READY_RESET
	cancel_work_sync(&rmi4_data->reset_work);
	flush_workqueue(rmi4_data->reset_workqueue);
	destroy_workqueue(rmi4_data->reset_workqueue);
#endif

	cancel_delayed_work_sync(&exp_data.work);
	flush_workqueue(exp_data.workqueue);
	destroy_workqueue(exp_data.workqueue);

	cancel_delayed_work_sync(&rmi4_data->rb_work);
	flush_workqueue(rmi4_data->rb_workqueue);
	destroy_workqueue(rmi4_data->rb_workqueue);

	for (attr_count = 0; attr_count < ARRAY_SIZE(attrs); attr_count++) {
		sysfs_remove_file(&rmi4_data->input_dev->dev.kobj,
				&attrs[attr_count].attr);
	}

#ifdef HTC_FEATURE
	synaptics_rmi4_sysfs_init(rmi4_data, false);
#endif

	if (rmi4_data->board_prop_dir) {
		sysfs_remove_file(rmi4_data->board_prop_dir,
				&virtual_key_map_attr.attr);
		kobject_put(rmi4_data->board_prop_dir);
	}

	synaptics_rmi4_irq_enable(rmi4_data, false, false);

#ifdef CONFIG_FB
	fb_unregister_client(&rmi4_data->fb_notifier);
#endif
#ifdef HTC_FEATURE
#ifdef CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY
	msm_drm_unregister_client(&rmi4_data->drm_notifier);
#endif
#endif

#ifdef USE_EARLYSUSPEND
	unregister_early_suspend(&rmi4_data->early_suspend);
#endif

#ifdef HTC_FEATURE
	if (rmi4_data->report_data != NULL)
		kfree(rmi4_data->report_data);
	if (rmi4_data->report_data_32 != NULL)
		kfree(rmi4_data->report_data_32);
#endif
	synaptics_rmi4_empty_fn_list(rmi4_data);
	input_unregister_device(rmi4_data->input_dev);
	rmi4_data->input_dev = NULL;
	if (rmi4_data->stylus_enable) {
		input_unregister_device(rmi4_data->stylus_dev);
		rmi4_data->stylus_dev = NULL;
	}

	synaptics_rmi4_gpio_setup(bdata->irq_gpio, false, 0, 0);

#ifdef HTC_FEATURE
	if (bdata->switch_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->switch_gpio, false, 0, 0);
#endif

	if (bdata->reset_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->reset_gpio, false, 0, 0);

	if (bdata->power_gpio >= 0)
		synaptics_rmi4_gpio_setup(bdata->power_gpio, false, 0, 0);

	synaptics_rmi4_enable_reg(rmi4_data, false);
	synaptics_rmi4_get_reg(rmi4_data, false);

	kfree(rmi4_data);

	return 0;
}

#ifdef HTC_FEATURE
#ifdef CONFIG_NANOHUB_TP_SWITCH
static void synaptics_switch_sensor_hub(struct synaptics_rmi4_data *rmi4_data, int mode)
{
	const struct synaptics_dsx_board_data *bdata =
				rmi4_data->hw_if->board_data;
	int mask = 0;

	if (bdata->support_glove)
		mask = (rmi4_data->glove_setting & 0x01) << 1;

	if (gpio_is_valid(bdata->switch_gpio)) {
		switch (mode) {
		case SWITCH_TO_CPU:
			nanohub_tp_status(mode | mask);
#ifdef CONFIG_NANOHUB_AOD
			nanohub_tp_mode(LCD_MODE_U0);
#endif
			gpio_direction_output(bdata->switch_gpio, !bdata->switch_hub_state);
			rmi4_data->i2c_to_mcu = SWITCH_TO_CPU;
			pr_info("[SensorHub] Switch touch i2c to CPU\n");
			break;
		case SWITCH_TO_MCU:
			if (bdata->support_eg_blk && rmi4_data->touch_detect == true) {
				rmi4_data->touch_detect = false;
				if (rmi4_data->debug_mask & TOUCH_DOWN_UP_LOG) {
					pr_info("%s: set touch_detect %s\n", __func__, rmi4_data->touch_detect ? "enabled" : "disabled");
				}
				report_touch_detect(rmi4_data);
			}
			gpio_direction_output(bdata->switch_gpio, bdata->switch_hub_state);
			rmi4_data->i2c_to_mcu = SWITCH_TO_MCU;
			pr_info("[SensorHub] Switch touch i2c to MCU\n");
#ifdef CONFIG_NANOHUB_AOD
			nanohub_tp_mode(LCD_MODE_U0);
#endif
			nanohub_tp_status(mode | mask);
			break;
		case SWITCH_TO_MCU_AOD:
			if (bdata->support_eg_blk && rmi4_data->touch_detect == true) {
				rmi4_data->touch_detect = false;
				if (rmi4_data->debug_mask & TOUCH_DOWN_UP_LOG) {
					pr_info("%s: set touch_detect %s\n", __func__, rmi4_data->touch_detect ? "enabled" : "disabled");
				}
				report_touch_detect(rmi4_data);
			}
			gpio_direction_output(bdata->switch_gpio, bdata->switch_hub_state);
			rmi4_data->i2c_to_mcu = SWITCH_TO_MCU_AOD;
			pr_info("[SensorHub] Switch touch i2c to MCU (AOD)\n");
#ifdef CONFIG_NANOHUB_AOD
			nanohub_tp_mode(LCD_MODE_U2);
#endif
			nanohub_tp_status((!!mode) | mask);
			break;
		default:
			pr_err("%s: incorrect mode %d\n", __func__, mode);
			break;
		}
	} else
		pr_err("%s: invalid switch gpio %d\n", __func__, bdata->switch_gpio);
}
#endif
#endif

#ifdef HTC_FEATURE
#ifdef CONFIG_TOUCHSCREEN_USE_DSI_NOTIFY
static void dsi_status_detect(int status)
{
	struct synaptics_rmi4_data *rmi4_data = exp_data.rmi4_data;
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	if (rmi4_data->debug_mask & TOUCH_DEBUG_LOG)
		pr_info("%s: dsi event %d\n", __func__, status);

	switch (status) {
	case LCM_EARLY_MIPI_OFF_CMD:
		break;
	case LCM_MIPI_OFF_CMD:
		synaptics_rmi4_suspend(&rmi4_data->pdev->dev);
		rmi4_data->fb_ready = false;
		break;
	case LCM_EARLY_POWERDOWN:
		break;
	case LCM_POWERDOWN:
		break;
	case LCM_REST_EARLY_LOW:
		break;
	case LCM_REST_LOW:
		break;
	case LCM_REST_EARLY_HIGH:
		if (bdata->lcm_reset_seq == 1) {
			synaptics_rmi4_hw_reset(rmi4_data);
			msleep(bdata->reset_lcm_delay_ms);
		}
		break;
	case LCM_REST_HIGH:
		break;
	default:
		break;
	};
}
#endif

#ifdef CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY
static int synaptics_rmi4_drm_notifier_cb(struct notifier_block *self,
		unsigned long event, void *data)
{
	int *transition;
	struct msm_drm_notifier *evdata = data;
	struct synaptics_rmi4_data *rmi4_data =
			container_of(self, struct synaptics_rmi4_data,
			drm_notifier);

#ifdef CONFIG_NANOHUB_TP_SWITCH
		const struct synaptics_dsx_board_data *bdata =
				rmi4_data->hw_if->board_data;
#endif

	pr_info("[DRM][%s-%d] mutex_lock rmi4_notifier_mutex\n",__func__,__LINE__);
	mutex_lock(&rmi4_data->rmi4_notifier_mutex);
	if (evdata && evdata->data && rmi4_data) {
		if (event == MSM_DRM_EARLY_EVENT_BLANK) {
			transition = evdata->data;
                        synaptics_secure_touch_stop(rmi4_data, 0);
			pr_info("DRM blank %d, event %ld, id %d\n", *transition, event, evdata->id);
			if (evdata->id != MSM_DRM_PRIMARY_DISPLAY)
				goto exit;
			if (*transition == MSM_DRM_BLANK_UNBLANK ||
					*transition == MSM_DRM_BLANK_STANDBY ||
					*transition == MSM_DRM_BLANK_POWERDOWN) {
#ifdef CONFIG_NANOHUB_TP_SWITCH
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_CPU);
#endif
			}
		} else if (event == MSM_DRM_EVENT_BLANK) {
			transition = evdata->data;
			pr_info("DRM blank %d, event %ld, id %d\n", *transition, event, evdata->id);
			if (evdata->id != MSM_DRM_PRIMARY_DISPLAY)
				goto exit;
			if (*transition == MSM_DRM_BLANK_POWERDOWN) {
#ifdef CONFIG_NANOHUB_TP_SWITCH
				if (bdata->lcm_reset_seq == 1) {
					gpio_direction_input(bdata->reset_gpio);
				}
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_MCU);
#endif
				rmi4_data->aod_mode = false;
			} else if (*transition == MSM_DRM_BLANK_UNBLANK) {
				synaptics_rmi4_resume(&rmi4_data->pdev->dev);
				rmi4_data->aod_mode = false;
			} else if (*transition == MSM_DRM_BLANK_STANDBY) {
#ifdef CONFIG_NANOHUB_TP_SWITCH
				if (bdata->lcm_reset_seq == 1) {
					gpio_direction_input(bdata->reset_gpio);
				}
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_MCU_AOD);
#endif
				rmi4_data->aod_mode = true;
			}
		} else if (event == MSM_DRM_REQUEST_EVENT_BLANK && rmi4_data->aod_mode) {
			transition = evdata->data;
			pr_info("DRM blank %d, event %ld, id %d\n", *transition, event, evdata->id);
			if (evdata->id != MSM_DRM_PRIMARY_DISPLAY)
				goto exit;
			if (*transition == MSM_DRM_BLANK_UNBLANK) {
#ifdef CONFIG_NANOHUB_TP_SWITCH
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_CPU);
#endif
				synaptics_rmi4_resume(&rmi4_data->pdev->dev);
				rmi4_data->aod_mode = false;
			}
		}else{
			transition = evdata->data;
			pr_info("[Not match]DRM blank %d, event %ld, id %d, aod mode %d\n", *transition, event, evdata->id,rmi4_data->aod_mode);
		}
	} else
		pr_info("FB event %ld\n", event);
exit:
	pr_info("[DRM][%s-%d] mutex_unlock rmi4_notifier_mutex\n",__func__,__LINE__);
	mutex_unlock(&rmi4_data->rmi4_notifier_mutex);
	return 0;
}
#endif
#endif

#ifdef CONFIG_FB
static int synaptics_rmi4_fb_notifier_cb(struct notifier_block *self,
		unsigned long event, void *data)
{
	int *transition;
	struct fb_event *evdata = data;
	struct synaptics_rmi4_data *rmi4_data =
			container_of(self, struct synaptics_rmi4_data,
			fb_notifier);
#ifdef HTC_FEATURE
#ifdef CONFIG_NANOHUB_TP_SWITCH
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;
#endif

	if (evdata && evdata->data && rmi4_data) {
		if (event == FB_EARLY_EVENT_BLANK) {
			transition = evdata->data;
			pr_info("FB blank %d, event %ld\n", *transition, event);
			if (*transition == FB_BLANK_UNBLANK ||
					*transition == FB_BLANK_NORMAL ||
					*transition == FB_BLANK_POWERDOWN) {
				//Need optimize AOD part (FB_BLANK_NORMAL)
#ifdef CONFIG_NANOHUB_TP_SWITCH
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_CPU);
//				synaptics_set_pmic(rmi4_data, false);		//pmic off
#endif
			}
		} else if (event == FB_EVENT_BLANK) {
			transition = evdata->data;
			pr_info("FB blank %d, event %ld\n", *transition, event);
			if (*transition == FB_BLANK_POWERDOWN) {
#ifdef CONFIG_NANOHUB_TP_SWITCH
				if (bdata->lcm_reset_seq == 1) {
					gpio_direction_input(bdata->reset_gpio);
				}
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_MCU);
#endif
				rmi4_data->aod_mode = false;
			} else if (*transition == FB_BLANK_UNBLANK) {
				synaptics_rmi4_resume(&rmi4_data->pdev->dev);
				rmi4_data->fb_ready = true;
				rmi4_data->aod_mode = false;
			} else if (*transition == FB_BLANK_NORMAL) {
				//Need optimize AOD part (FB_BLANK_NORMAL)
#ifdef CONFIG_NANOHUB_TP_SWITCH
				if (bdata->lcm_reset_seq == 1) {
					gpio_direction_input(bdata->reset_gpio);
				}
				synaptics_switch_sensor_hub(rmi4_data, SWITCH_TO_MCU_AOD);
#endif
				rmi4_data->aod_mode = true;
			}
		}
	} else
		pr_info("FB event %ld\n", event);
#else
	if (evdata && evdata->data && rmi4_data) {
		if (event == FB_EVENT_BLANK) {
			transition = evdata->data;
			if (*transition == FB_BLANK_POWERDOWN) {
				synaptics_rmi4_suspend(&rmi4_data->pdev->dev);
				rmi4_data->fb_ready = false;
			} else if (*transition == FB_BLANK_UNBLANK) {
				synaptics_rmi4_resume(&rmi4_data->pdev->dev);
				rmi4_data->fb_ready = true;
			}
		}
	}
#endif

	return 0;
}
#endif

#ifdef USE_EARLYSUSPEND
static void synaptics_rmi4_early_suspend(struct early_suspend *h)
{
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_data *rmi4_data =
			container_of(h, struct synaptics_rmi4_data,
			early_suspend);
	unsigned char device_ctrl;

	if (rmi4_data->stay_awake)
		return;

        synaptics_secure_touch_stop(rmi4_data, 0);

	if (rmi4_data->enable_wakeup_gesture) {
		if (rmi4_data->no_sleep_setting) {
			synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f01_ctrl_base_addr,
					&device_ctrl,
					sizeof(device_ctrl));
			device_ctrl = device_ctrl & ~NO_SLEEP_ON;
			synaptics_rmi4_reg_write(rmi4_data,
					rmi4_data->f01_ctrl_base_addr,
					&device_ctrl,
					sizeof(device_ctrl));
		}
		synaptics_rmi4_wakeup_gesture(rmi4_data, true);
		enable_irq_wake(rmi4_data->irq);
		goto exit;
	}

#ifdef SYNA_TDDI
	if (rmi4_data->no_sleep_setting) {
		synaptics_rmi4_reg_read(rmi4_data,
				rmi4_data->f01_ctrl_base_addr,
				&device_ctrl,
				sizeof(device_ctrl));
		device_ctrl = device_ctrl & ~NO_SLEEP_ON;
		synaptics_rmi4_reg_write(rmi4_data,
				rmi4_data->f01_ctrl_base_addr,
				&device_ctrl,
				sizeof(device_ctrl));
	}
	synaptics_rmi4_wakeup_gesture(rmi4_data, true);
	usleep(TDDI_LPWG_WAIT_US);
#endif
	synaptics_rmi4_irq_enable(rmi4_data, false, false);
	synaptics_rmi4_sleep_enable(rmi4_data, true);
	synaptics_rmi4_free_fingers(rmi4_data);

exit:
	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->early_suspend != NULL)
				exp_fhandler->exp_fn->early_suspend(rmi4_data);
	}
	mutex_unlock(&exp_data.mutex);

	rmi4_data->suspend = true;

	return;
}

static void synaptics_rmi4_late_resume(struct early_suspend *h)
{
#ifdef FB_READY_RESET
	int retval;
#endif
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_data *rmi4_data =
			container_of(h, struct synaptics_rmi4_data,
			early_suspend);

	if (rmi4_data->stay_awake)
		return;

        synaptics_secure_touch_stop(rmi4_data, 0);

	if (rmi4_data->enable_wakeup_gesture) {
		disable_irq_wake(rmi4_data->irq);
		goto exit;
	}

	rmi4_data->current_page = MASK_8BIT;

	if (rmi4_data->suspend) {
		synaptics_rmi4_sleep_enable(rmi4_data, false);
		synaptics_rmi4_irq_enable(rmi4_data, true, false);
	}

exit:
#ifdef FB_READY_RESET
	if (rmi4_data->suspend) {
		retval = synaptics_rmi4_reset_device(rmi4_data, false);
		if (retval < 0) {
			dev_err(rmi4_data->pdev->dev.parent,
					"%s: Failed to issue reset command\n",
					__func__);
		}
	}
#endif
	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->late_resume != NULL)
				exp_fhandler->exp_fn->late_resume(rmi4_data);
	}
	mutex_unlock(&exp_data.mutex);

	rmi4_data->suspend = false;

	return;
}
#endif

static int synaptics_rmi4_suspend(struct device *dev)
{
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);
	unsigned char device_ctrl;
#ifdef HTC_FEATURE
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;
#endif

	pr_info("%s\n", __func__);

	if (rmi4_data->stay_awake)
		return 0;

	if (rmi4_data->enable_wakeup_gesture) {
		if (rmi4_data->no_sleep_setting) {
			synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f01_ctrl_base_addr,
					&device_ctrl,
					sizeof(device_ctrl));
			device_ctrl = device_ctrl & ~NO_SLEEP_ON;
			synaptics_rmi4_reg_write(rmi4_data,
					rmi4_data->f01_ctrl_base_addr,
					&device_ctrl,
					sizeof(device_ctrl));
		}
		synaptics_rmi4_wakeup_gesture(rmi4_data, true);
		enable_irq_wake(rmi4_data->irq);
		goto exit;
	}

	if (!rmi4_data->suspend) {
#ifdef SYNA_TDDI
		if (rmi4_data->no_sleep_setting) {
			synaptics_rmi4_reg_read(rmi4_data,
					rmi4_data->f01_ctrl_base_addr,
					&device_ctrl,
					sizeof(device_ctrl));
			device_ctrl = device_ctrl & ~NO_SLEEP_ON;
			synaptics_rmi4_reg_write(rmi4_data,
					rmi4_data->f01_ctrl_base_addr,
					&device_ctrl,
					sizeof(device_ctrl));
		}
		synaptics_rmi4_wakeup_gesture(rmi4_data, true);
		usleep(TDDI_LPWG_WAIT_US);
#endif
#ifdef HTC_FEATURE
		if (bdata->sensor_sleep_mode == 1) {
			synaptics_set_pmic(rmi4_data, false);
			if (bdata->set_pmic_delay)
				msleep(bdata->set_pmic_delay);
			synaptics_rmi4_irq_enable(rmi4_data, false, false);
			synaptics_rmi4_wakeup_gesture(rmi4_data, true);
			if (bdata->set_wg_delay)
				msleep(bdata->set_wg_delay);
			synaptics_rmi4_sleep_enable(rmi4_data, true);
			if (bdata->set_sleep_delay)
				msleep(bdata->set_sleep_delay);
		} else {
			synaptics_rmi4_irq_enable(rmi4_data, false, false);
			synaptics_rmi4_sleep_enable(rmi4_data, true);
		}
	} else if (rmi4_data->aod_mode) {	//AOD mode
		if (bdata->sensor_sleep_mode == 1) {
			synaptics_set_pmic(rmi4_data, false);
			if (bdata->set_pmic_delay)
				msleep(bdata->set_pmic_delay);
			synaptics_rmi4_irq_enable(rmi4_data, false, false);
			synaptics_rmi4_wakeup_gesture(rmi4_data, true);
			if (bdata->set_wg_delay)
				msleep(bdata->set_wg_delay);
			synaptics_rmi4_sleep_enable(rmi4_data, true);
			if (bdata->set_sleep_delay)
				msleep(bdata->set_sleep_delay);
		}
#else
		synaptics_rmi4_irq_enable(rmi4_data, false, false);
		synaptics_rmi4_sleep_enable(rmi4_data, true);
		synaptics_rmi4_free_fingers(rmi4_data);
#endif
	}

        synaptics_secure_touch_stop(rmi4_data, 1);

exit:
	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->suspend != NULL)
				exp_fhandler->exp_fn->suspend(rmi4_data);
	}
	mutex_unlock(&exp_data.mutex);

	rmi4_data->suspend = true;

	return 0;
}

static int synaptics_rmi4_resume(struct device *dev)
{
#ifdef FB_READY_RESET
	int retval;
#endif
	struct synaptics_rmi4_exp_fhandler *exp_fhandler;
	struct synaptics_rmi4_data *rmi4_data = dev_get_drvdata(dev);
#ifdef HTC_FEATURE
	const struct synaptics_dsx_board_data *bdata =
			rmi4_data->hw_if->board_data;

	pr_info("%s: PR%d-%s\n", __func__, rmi4_data->firmware_id, rmi4_data->config_id);
#endif

	if (rmi4_data->stay_awake)
		return 0;

        synaptics_secure_touch_stop(rmi4_data, 1);

	if (rmi4_data->enable_wakeup_gesture) {
		disable_irq_wake(rmi4_data->irq);
		goto exit;
	}

	rmi4_data->current_page = MASK_8BIT;

	synaptics_rmi4_sleep_enable(rmi4_data, false);
#ifdef HTC_FEATURE
	if (bdata->lcm_reset_seq == 1)
		synaptics_rmi4_set_configured(rmi4_data);
#endif
	synaptics_rmi4_irq_enable(rmi4_data, true, false);

exit:

#ifdef HTC_FEATURE
	synaptics_rmi4_free_fingers(rmi4_data);
	if (!bdata->lcm_reset_seq)
		synaptics_rmi4_hw_reset(rmi4_data);
	else if (bdata->lcm_reset_seq == 1 && rmi4_data->aod_mode) {
		synaptics_rmi4_hw_reset(rmi4_data);
		mutex_lock(&(rmi4_data->rmi4_irq_enable_mutex));
		if (!rmi4_data->irq_enabled) {
			enable_irq(rmi4_data->irq);
			rmi4_data->irq_enabled = true;
		}
		mutex_unlock(&(rmi4_data->rmi4_irq_enable_mutex));
	}
#else
#ifdef FB_READY_RESET
	retval = synaptics_rmi4_reset_device(rmi4_data, false);
	if (retval < 0) {
		dev_err(rmi4_data->pdev->dev.parent,
				"%s: Failed to issue reset command\n",
				__func__);
	}
#endif
#endif
	mutex_lock(&exp_data.mutex);
	if (!list_empty(&exp_data.list)) {
		list_for_each_entry(exp_fhandler, &exp_data.list, link)
			if (exp_fhandler->exp_fn->resume != NULL)
				exp_fhandler->exp_fn->resume(rmi4_data);
	}
	mutex_unlock(&exp_data.mutex);

#ifdef HTC_FEATURE
	if (bdata->lcm_reset_seq == 1 && !rmi4_data->aod_mode
			&& bdata->support_glove && rmi4_data->glove_setting)
		synaptics_rmi4_set_mode_reg(rmi4_data);
#endif

	rmi4_data->suspend = false;

	return 0;
}

#ifdef CONFIG_PM
static const struct dev_pm_ops synaptics_rmi4_dev_pm_ops = {
#ifndef CONFIG_FB
#if defined(HTC_FEATURE) && !defined(CONFIG_TOUCHSCREEN_USE_DRM_NOTIFY)
	.suspend = synaptics_rmi4_suspend,
	.resume = synaptics_rmi4_resume,
#endif
#endif
};
#endif

static struct platform_driver synaptics_rmi4_driver = {
	.driver = {
		.name = PLATFORM_DRIVER_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_PM
		.pm = &synaptics_rmi4_dev_pm_ops,
#endif
	},
	.probe = synaptics_rmi4_probe,
	.remove = synaptics_rmi4_remove,
};

#ifdef HTC_FEATURE
static void __init synaptics_rmi4_init_async(void *unused, async_cookie_t cookie)
{
	pr_info("%s ++\n", __func__);
	if (synaptics_rmi4_bus_init())
		return;

	platform_driver_register(&synaptics_rmi4_driver);
	pr_info("%s --\n", __func__);
	return;
}

static int __init synaptics_rmi4_init(void)
{
	async_schedule(synaptics_rmi4_init_async, NULL);
	return 0;
}
#else
static int __init synaptics_rmi4_init(void)
{
	int retval;

	retval = synaptics_rmi4_bus_init();
	if (retval)
		return retval;

	return platform_driver_register(&synaptics_rmi4_driver);
}
#endif

static void __exit synaptics_rmi4_exit(void)
{
	platform_driver_unregister(&synaptics_rmi4_driver);

	synaptics_rmi4_bus_exit();

	return;
}

#ifdef HTC_FEATURE
static int __init get_tamper_flag(char *str)
{
	int ret = kstrtouint(str, 0, &tamper_flag);
	pr_info(" %s: tamper_flag: %d, ret : %d", __func__, tamper_flag, ret);
	return ret;
} early_param("td.sf", get_tamper_flag);
#endif

module_init(synaptics_rmi4_init);
module_exit(synaptics_rmi4_exit);

MODULE_AUTHOR("Synaptics, Inc.");
MODULE_DESCRIPTION("Synaptics DSX Touch Driver");
MODULE_LICENSE("GPL v2");
