#!/usr/bin/bash
make -C ../ &&
sudo cp ../bin/sms /usr/local/bin/sms &&
sudo mkdir -p /usr/lib/sms &&
sudo cp -fr ../sms_src/lib/* /usr/lib/sms &&
sudo chmod -R 445 /usr/lib/sms
export SMS_PATH=/usr/lib/sms

