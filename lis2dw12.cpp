
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include "lis2dw12.h"
#include "sim_data.h"

const struct device *const sensor = DEVICE_DT_GET_ONE(st_lis2dw12);

struct th_val {
	uint8_t	apply;
	int32_t mg;
};


static struct th_val th_mgValue[5] ={{5,8158},{6, 9789},{7,11421},{8,13052},{9,14684}};
static uint8_t th_idx = 4;
#define CONFIG_SHOCK_THRESHOLD_MS2_PPM 16000000


static void on_shock_detected(const struct device *dev,
					 	const struct sensor_trigger *trigger)
{
    printk("shock");
}	

static int acc_threshold_set(int64_t threshold)
{

	struct sensor_value odr_attr, fs_attr;
	struct sensor_value value;
	int err;

	do
	{ 
		value.val1 = (int)(threshold / 1000000LL);
		value.val2 = (int)((threshold - value.val1*1000000L));
		odr_attr.val1 = 100;
		odr_attr.val2 = 0;

		//err = sensor_attr_set(sensor, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);

		//sensor_g_to_ms2(2, &fs_attr);
		//err = sensor_attr_set(sensor, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_FULL_SCALE, &fs_attr);

	} while (0);
	
	return err;
}

static int acc_threshold_set_org(int64_t threshold)
{
	static const struct sensor_trigger wakeup_trig = {
		.type = SENSOR_TRIG_THRESHOLD,
		.chan = SENSOR_CHAN_ACCEL_XYZ,
	};

	struct sensor_value value;
	int err;

	//UNUSED_VARIABLE(wakeup_trig);
	// HW팀 요청에 따라서 impact 값을 아래와 같이 변경함. 적용자 HW황수석님, SW박수석님
	threshold *= th_mgValue[th_idx].apply; 

	do
	{ 
		value.val1 = (int)(threshold / 1000000LL);
		value.val2 = (int)((threshold - value.val1*1000000L));

		err = sensor_attr_set(sensor, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_UPPER_THRESH, &value);
		if(err !=0) break;

		err = sensor_trigger_set(sensor, &wakeup_trig, on_shock_detected);
		if(err !=0) break;
	} while (0);
	
	return err;
}


static void on_data_ready(const struct device *dev,
					 	const struct sensor_trigger *trigger)
{
	drv_acc_value_t acc;
    char buff[200];
	memset(&acc, 0x00, sizeof(acc));

    acc_get(&acc);
    
    sprintf(buff, "%lld, %f, %f, %f\n", 
		k_uptime_get(),
		acc.x, acc.y, acc.z
	);
    printk("%s", buff	);
}

static int acc_drdy_set(void)
{
	int err;
	static const struct sensor_trigger trig = {
		.type = SENSOR_TRIG_DATA_READY,
	};

	err = sensor_trigger_set(sensor, &trig, on_data_ready);

	
	return err;
}

int acc_init(void)
{
	int err;

	if (!device_is_ready(sensor))
	{
		printk("device not ready.\n");
		return -EACCES;
	}

	if (IS_ENABLED(CONFIG_APP_FACTORY_ACC_DATA))
		err = acc_drdy_set();	
	else
	    err = acc_threshold_set(CONFIG_SHOCK_THRESHOLD_MS2_PPM);

	return err;
}


int acc_get(drv_acc_value_t* p_value) {
    
	if (!device_is_ready(sensor))
	{
		printk("ACC sensor is not ready");
		return -EACCES;
	}

	struct sensor_value temp[3];
	int err;

	do
	{
		err = sensor_sample_fetch(sensor);
		if (err)
		{
			printk("ACC sensor fetch error");
			break;
		}

		err = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, temp);
		if (err)
		{
			printk("ACC sensor channel get error");
			break;
		}

		p_value->x = sensor_value_to_double(&temp[0]);
		p_value->y = sensor_value_to_double(&temp[1]);
		p_value->z = sensor_value_to_double(&temp[2]);

		//LOG_DBG("LIS2DW12: %f %f %f", p_value->x, p_value->y, p_value->z);
	} while (0);	

	return err;
}

void acc_get_data(int index, drv_acc_value_t* p_value) {
    p_value->x = ring_data[index][0];
    p_value->y = ring_data[index][1];
    p_value->z = ring_data[index][2];
}
