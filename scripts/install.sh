#!/usr/bin/bash
make -C ../ &&
sudo cp ../bin/sms /usr/local/bin/sms &&
export SMS_PATH=`realpath ../sms_src/lib`

