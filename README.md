# JZ-F407VET6_Zephyr_Zenoh
JZ-F407VET6 With Zephyr RTOS and Zenoh

# Requirements

Install west

```bash
pip3 install west
```

Install dependencies

```bash
west update
```

# Build publisher

Go to the publisher folder and run:

```bash
cd app_jz407/publisher 
west build -p -b JZ_F407VET6 -s . -- -DBOARD_ROOT=$PWD/..
west flash 
```