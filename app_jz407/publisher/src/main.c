#include <stdio.h>
#include <string.h>
#include <zenoh-pico.h>
#include <unistd.h>

int publish_json(z_owned_session_t *session, z_owned_publisher_t *pub, const char *json)
{
  z_owned_bytes_t payload;
  z_bytes_copy_from_str(&payload, json);
  return z_publisher_put(z_loan(*pub), z_move(payload), NULL);
}

int main(void)
{
  while (true)
  {
    sleep(2);
    z_owned_config_t config;
    z_config_default(&config);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, "client");
    zp_config_insert(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, "tcp/192.168.12.103:7447");
  
    z_owned_session_t s;
    if (z_open(&s, z_move(config), NULL) < 0)
    {
      printk("Unable to open session!\n");
      continue;
    }
  
    zp_start_read_task(z_loan_mut(s), NULL);
    zp_start_lease_task(z_loan_mut(s), NULL);
  
    z_view_keyexpr_t ke_str, ke_imu, ke_bat;
    z_view_keyexpr_from_str_unchecked(&ke_str, "rt/status/string");
    z_view_keyexpr_from_str_unchecked(&ke_imu, "rt/sensor/imu");
    z_view_keyexpr_from_str_unchecked(&ke_bat, "rt/power/battery");
  
    z_owned_publisher_t pub_str, pub_imu, pub_bat;

    if (z_declare_publisher(z_loan(s), &pub_str, z_loan(ke_str), NULL) < 0) {
      printk("Unable to declare publisher for string topic!\n");
      continue;
    }

    if (z_declare_publisher(z_loan(s), &pub_imu, z_loan(ke_imu), NULL) < 0) {
      printk("Unable to declare publisher for imu tompic!\n");
      continue;
    }

    if (z_declare_publisher(z_loan(s), &pub_bat, z_loan(ke_bat), NULL) < 0) {
      printk("Unable to declare publisher for battery topic!\n");
      continue;
    }
  
    int counter = 0;
    printk("Ok!\n");
    while (1)
    {
      // 1. Mensaje tipo String
      char msg_buf[64];
      snprintf(msg_buf, sizeof(msg_buf), "{\"data\": \"Hi everyone! [%d]\"}", counter);
      if (publish_json(&s, &pub_str, msg_buf) < 0) {
        printk("Unable to publish message!\n");
        continue;
      }
  
      // 2. IMU JSON payload
      const char *imu_json =
          "{"
          "\"orientation\": {\"x\": 0.1, \"y\": 0.2, \"z\": 0.3, \"w\": 1.0},"
          "\"angular_velocity\": {\"x\": 0.01, \"y\": 0.02, \"z\": 0.03},"
          "\"linear_acceleration\": {\"x\": 9.8, \"y\": 0.0, \"z\": 0.0}"
          "}";
  
      if (publish_json(&s, &pub_imu, imu_json) < 0) {
        printk("Unable to publish message!\n");
        continue;
      }
      // 3. BatteryState JSON payload
      const char *bat_json =
          "{"
          "\"voltage\": 11.1,"
          "\"current\": 1.2,"
          "\"percentage\": 0.75"
          "}";
  
      if (publish_json(&s, &pub_bat, bat_json) < 0) {
        printk("Unable to publish message!\n");
        continue;
      }
  
      counter++;
      sleep(1);
    }
  }
  return 0;
}
