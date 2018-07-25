/*
 * driver for EETI Multi-Touch controller - VAL130
 * (EXC79xx/3000/31XX/80Hxx/82Hxx)
 * 
 * based on egalax_ts.c, minimalist single touch implementaion
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/input/mt.h>
#include <linux/of_gpio.h>


#define DVALID          0x42
#define I2C_DATA_LEN    0x43 
#define EVENT_DOWN_UP	(0X1 << 0)
#define EGALAX_MAX_X	4096
#define EGALAX_MAX_Y	4096

struct val130_ts {
	struct i2c_client		*client;
	struct input_dev		*input_dev;
};

static irqreturn_t val130_ts_interrupt(int irq, void *dev_id)
{
	struct val130_ts *ts = dev_id;
	struct input_dev *input_dev = ts->input_dev;
	struct i2c_client *client = ts->client;
	u8 buf[I2C_DATA_LEN];
	int id, ret, x, y, z;
	bool down;
	u8 state;


    ret = i2c_master_recv(client, buf, I2C_DATA_LEN);
    //print_hex_dump_bytes("data:", DUMP_PREFIX_NONE, buf, I2C_DATA_LEN);

	if (ret < 0)
		return IRQ_HANDLED;

	if (buf[0] != DVALID) {
		/*the read data is invalied or duplicate*/
		return IRQ_HANDLED;
	}
	state = buf[4]; /* finger down=1, finger up=0*/
	x = (buf[7] << 8) | buf[6];
	y = (buf[9] << 8) | buf[8];
	down = state & EVENT_DOWN_UP;
	dev_dbg(&client->dev, "%s id:%d x:%d y:%d z:%d",
		down ? "down" : "up", id, x, y, z);

	if (down) {
		input_report_abs(input_dev, ABS_MT_POSITION_X, x);
		input_report_abs(input_dev, ABS_MT_POSITION_Y, y);
	}
	input_sync(input_dev);
	return IRQ_HANDLED;
}

/* wake up controller by an falling edge of interrupt gpio.  */
static int val130_wake_up_device(struct i2c_client *client)
{
	struct device_node *np = client->dev.of_node;
	int gpio;
	int ret;

	if (!np)
		return -ENODEV;

	gpio = of_get_named_gpio(np, "wakeup-gpios", 0);
	if (!gpio_is_valid(gpio))
		return -ENODEV;

	ret = gpio_request(gpio, "val130_irq");
	if (ret < 0) {
		dev_err(&client->dev,
			"request gpio failed, cannot wake up controller: %d\n",
			ret);
		return ret;
	}

	/* wake up controller via an falling edge on IRQ gpio. */
	gpio_direction_output(gpio, 0);
	gpio_set_value(gpio, 1);

	/* controller should be waken up, return irq.  */
	gpio_direction_input(gpio);
	gpio_free(gpio);

	return 0;
}



static int val130_ts_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct val130_ts *ts;
	struct input_dev *input_dev;
	int error;

	/* controller may be in sleep, wake it up. */
	error = val130_wake_up_device(client);
	if (error) {
		dev_err(&client->dev, "Failed to wake up the controller\n");
		return error;
	}


	ts = devm_kzalloc(&client->dev, sizeof(struct val130_ts), GFP_KERNEL);
	if (!ts) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	input_dev = devm_input_allocate_device(&client->dev);
	if (!input_dev) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	ts->client = client;
	ts->input_dev = input_dev;
   
	input_dev->name = "VAL130 touchscreen";
	input_dev->id.bustype = BUS_I2C;

	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(BTN_TOUCH, input_dev->keybit);

	input_set_abs_params(input_dev, ABS_X, 0, EGALAX_MAX_X, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, EGALAX_MAX_Y, 0, 0);
	input_set_abs_params(input_dev,
			     ABS_MT_POSITION_X, 0, EGALAX_MAX_X, 0, 0);
	input_set_abs_params(input_dev,
			     ABS_MT_POSITION_Y, 0, EGALAX_MAX_Y, 0, 0);
	input_set_drvdata(input_dev, ts);
	error = devm_request_threaded_irq(&client->dev, client->irq, NULL,
					  val130_ts_interrupt,
					  IRQF_TRIGGER_LOW | IRQF_ONESHOT,
					  "val130_ts", ts);
	if (error < 0) {
		dev_err(&client->dev, "Failed to register interrupt\n");
		return error;
	}

	error = input_register_device(ts->input_dev);
	if (error)
		return error;

	i2c_set_clientdata(client, ts);
	return 0;
}

static const struct i2c_device_id val130_ts_id[] = {
	{ "val130_ts", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, val130_ts_id);



static const struct of_device_id val130_ts_dt_ids[] = {
	{ .compatible = "val130_ts" },
	{ /* sentinel */ }
};

static struct i2c_driver val130_ts_driver = {
	.driver = {
		.name	= "val130_ts",
		.of_match_table	= val130_ts_dt_ids,
	},
	.id_table	= val130_ts_id,
	.probe		= val130_ts_probe,
};

module_i2c_driver(val130_ts_driver);

MODULE_AUTHOR("Access Interface Solutions");
MODULE_DESCRIPTION("Touchscreen driver for VAL130");
MODULE_LICENSE("GPL");
