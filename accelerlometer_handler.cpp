/*
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accelerometer_handler.hpp"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include "lis2dw12.h"
#include "graph.h"

void extract_x(float* input, float* output);
void extract_y(float* input, float* output);
void extract_z(float* input, float* output);

#define BUFLEN 300
int begin_index = 0;
const struct device *const sensor = DEVICE_DT_GET_ONE(st_lis2dw12);
int current_index = 0;

float bufx[BUFLEN] = { 0.0f };
float bufy[BUFLEN] = { 0.0f };
float bufz[BUFLEN] = { 0.0f };

bool initial = true;


TfLiteStatus SetupAccelerometer(tflite::ErrorReporter *error_reporter)
{
	if (!device_is_ready(sensor)) {
		printk("%s: device not ready.\n", sensor->name);
		return kTfLiteApplicationError;
	}
	acc_init();
	
	if (sensor == NULL) {
		TF_LITE_REPORT_ERROR(error_reporter,
				     "Failed to get accelerometer, name: %s\n",
				     sensor->name);
	} else {
		TF_LITE_REPORT_ERROR(error_reporter, "Got accelerometer, name: %s\n",
				     sensor->name);
	}
	return kTfLiteOk;
}

bool ReadAccelerometer(tflite::ErrorReporter *error_reporter, float *input,
		       int length)
{

	int rc;
	struct sensor_value accel[3];
	int samples_count;
	float x,y,z;
	char str[100];
	int weight=100;
	float output[200];


	samples_count = 80;

	if (!device_is_ready(sensor))
	{
		//LOG_ERR("ACC sensor is not ready");
		TF_LITE_REPORT_ERROR(error_reporter, "ACC sensor is not ready\n");
	}

	for (int i = 0; i < samples_count; ) {
		
		sensor_sample_fetch(sensor);

		rc = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
		if (rc < 0) {
			TF_LITE_REPORT_ERROR(error_reporter, "ERROR: Update failed: %d\n", rc);
			return false;
		}

		x = (float)sensor_value_to_double(&accel[0])*weight;
		y = (float)sensor_value_to_double(&accel[1])*weight;
		z = (float)(sensor_value_to_double(&accel[2])-9.8)*weight; // 중력가속도 9.8을 빼준다. adl adxl345와 맞추기 위해

		// 이전값과 다른 경우에만 update하고 이전값과 같으면 pass한다
		if(bufx[begin_index-1] != x || bufy[begin_index-1] != y || bufz[begin_index-1] != z) {
			bufx[begin_index] = (x);
			bufy[begin_index] = (y);
			bufz[begin_index] = (z);

			sprintf(str,"%.1f,%.1f,%.1f\n",bufx[begin_index],bufy[begin_index],bufz[begin_index]);
			printk("%s",str);
			
			begin_index++;
			if (begin_index >= BUFLEN) {
				begin_index = 0;
			}
			i++;
		}
		K_MSEC(20);
	}

	if (initial && begin_index >= 100) {
		initial = false;
	}

	if (initial) {
		TF_LITE_REPORT_ERROR(error_reporter, "acc initial fail");
		return false;
	}

	// length 384
	int sample = 0;
	for (int i = 0; i < (length - 3); i += 3) {
		int ring_index = begin_index + sample - length / 3;
		if (ring_index < 0) {
			ring_index += BUFLEN;
		}
		input[i] = (bufx[ring_index]);
		input[i + 1] = (bufy[ring_index]);
		input[i + 2] = (bufz[ring_index]);
		//sprintf(str,"%.1f, %.1f, %.1f\n",input[i],input[i + 1],input[i + 2]);
		//printk("%s",str);
		sample++;
	}

	init_cordi(100,130);
	extract_x(input,output);
	fill_x_cordi(output);
	//fill_x_cordi(&bufx[0]);
	print_cordi();

	init_cordi(100,130);
	extract_z(input,output);
	fill_x_cordi(output);
	//fill_x_cordi(&bufz[0]);
	print_cordi();

	return true;
}

// ring 패턴은 확인했음 240723
bool ReadAccelerometer_test(tflite::ErrorReporter *error_reporter, float *input,
		       int length)
{
	int samples_count;
	float x,y,z;
	char str[100];
	int weight=1;
	drv_acc_value_t acc;
	float output[200];

	samples_count = 73;
	for (int i = 0; i < samples_count; ) {
		
		memset(&acc, 0x00, sizeof(acc));
		//acc_get(&acc);
		acc_get_data(i,&acc);
		x = acc.x*weight;
		y = acc.y*weight;
		z = (acc.z)*weight; // 중력가속도 9.8을 빼준다? adl adxl345와 맞추기 위해?

		// 이전값과 다른 경우에만 update하고 이전값과 같으면 pass한다
		if(bufx[begin_index-1] != x || bufy[begin_index-1] != y || bufz[begin_index-1] != z) {
			bufx[begin_index] = x;
			bufy[begin_index] = y;
			bufz[begin_index] = z;

			sprintf(str,"%.1f, %.1f, %.1f\n",bufx[begin_index],bufy[begin_index],bufz[begin_index]);
			printk("%s",str);
			
			begin_index++;
			if (begin_index >= BUFLEN) {
				begin_index = 0;
			}
			i++;
		}
	}

	if (initial && begin_index >= 100) {
		initial = false;
	}

	if (initial) {
		TF_LITE_REPORT_ERROR(error_reporter, "acc initial fail");
		return false;
	}

	// length 384
	int sample = 0;
	for (int i = 0; i < (length - 3); i += 3) {
		int ring_index = begin_index + sample - length / 3;
		if (ring_index < 0) {
			ring_index += BUFLEN;
		}
		input[i] = bufx[ring_index];
		input[i + 1] = bufy[ring_index];
		input[i + 2] = bufz[ring_index];
		//sprintf(str,"%.1f, %.1f, %.1f\n",input[i],input[i + 1],input[i + 2]);
		//printk("%s",str);
		sample++;
	}

	init_cordi(100,130);
	extract_x(input,output);
	fill_x_cordi(output);
	//fill_x_cordi(&bufx[0]);
	print_cordi();

	init_cordi(100,130);
	extract_z(input,output);
	fill_x_cordi(output);
	//fill_x_cordi(&bufz[0]);
	print_cordi();


	return true;
}

void extract_x(float* input, float* output) {
	int j=0;
	for(int i =0;j<150;i+=3) {
		output[j] =input[i];
		j++;
	}
}
void extract_y(float* input, float* output) {
	int j=0;
	for(int i =0;j<150;i+=3) {
		output[j] =input[i+1];
		j++;
	}
}
void extract_z(float* input, float* output) {
	int j=0;
	for(int i =0;j<150;i+=3) {
		output[j] =input[i+2];
		j++;
	}
}
