"""
    This is from
    https://www.instructables.com/How-to-Find-Heading-Angle-From-MPU9250-With-Raspbe/
    library is:
    https://github.com/kevinmcaleer/mpu9250
"""
from machine import I2C, Pin
import math
import utime

from mpu9250 import MPU9250

i2c = I2C(0, sda=Pin(4), scl=Pin(5), freq=400000)
sensor = MPU9250(i2c)
filtered_magx, filtered_magy = 0, 0
DECLINATION = -1 * 6.20

# Want to thank Kevin McAleer

def low_pass_filter(prev_value, new_value):
    return 0.85 * prev_value + 0.15 * new_value


def compass(angle):
    if angle > 337 or angle <= 22:
        direction = 'North'
    elif angle > 22 and angle <= 67:
        direction = 'North East'
    elif angle > 67 and angle <= 112:
        direction = "East"
    elif angle > 112 and angle <= 157:
        direction = "South East"
    elif angle > 157 and angle <= 202:
        direction = "South"
    elif angle > 202 and angle <= 247:
        direction = "South West"
    elif angle > 247 and angle <= 292:
        direction = "West"
    elif angle > 292 and angle <= 337:
        direction = "North West"
    return direction


#sensor.ak8963.calibrate()

while True:

    # We do not need the z value
    magx_new, magy_new, _ = sensor.magnetic
    
    filtered_magx = low_pass_filter(filtered_magx, magx_new)
    filtered_magy = low_pass_filter(filtered_magy, magy_new)
    
    heading_angle_in_degrees = math.atan2(filtered_magx, filtered_magy) * (180 / math.pi)
    heading_angle_in_degrees_plus_declination = heading_angle_in_degrees + DECLINATION
    
    if heading_angle_in_degrees_plus_declination < 0:
        heading_angle_in_degrees += 360
        heading_angle_in_degrees_plus_declination += 360
    
    print('###Without Declination###')
    print(heading_angle_in_degrees)
    print(compass(heading_angle_in_degrees))
    print('###Plus Declination###')
    print(heading_angle_in_degrees_plus_declination)
    print(compass(heading_angle_in_degrees_plus_declination))
    
    utime.sleep_ms(100)
