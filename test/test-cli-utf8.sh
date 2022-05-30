#!/bin/bash

: ${PREFIX=c89-char}

export PATH=/🍕
TEST="${PREFIX}/way bytes";([ "`${TEST}`" == "5" ] || echo "FAILURE: ${TEST}")
TEST="${PREFIX}/way chars";([ "`${TEST}`" == "2" ] || echo "FAILURE: ${TEST}")
[ "`${PREFIX}/way count`" == "1" ] || echo "FAILURE"

[ "`${PREFIX}/way get`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 1`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 1`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -t`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get --tail`" == "/🍕" ] || echo "FAILURE"

[ "`${PREFIX}/way delete`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 0`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 0`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -t`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --tail`" == "" ] || echo "FAILURE"

[ "`${PREFIX}/way insert /☯️`" == "/☯️:/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 0 /☯️`" == "/☯️:/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 0 /☯️`" == "/☯️:/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 1 /☯️`" == "/🍕:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 1 /☯️`" == "/🍕:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 2 /☯️`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 2 /☯️`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -t /☯️`" == "/🍕:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --tail /☯️`" == "/🍕:/☯️" ] || echo "FAILURE"

export PATH=/🍕:/🦎
TEST="${PREFIX}/way bytes";([ "`${TEST}`" == "11" ] || echo "FAILURE: ${TEST}")
TEST="${PREFIX}/way chars";([ "`${TEST}`" == "5" ] || echo "FAILURE: ${TEST}")
[ "`${PREFIX}/way count`" == "2" ] || echo "FAILURE"

[ "`${PREFIX}/way get`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 1`" == "/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 1`" == "/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 2`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 2`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -t`" == "/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way get --tail`" == "/🦎" ] || echo "FAILURE"

[ "`${PREFIX}/way delete`" == "/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 0`" == "/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 0`" == "/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -t`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --tail`" == "/🍕" ] || echo "FAILURE"

[ "`${PREFIX}/way insert /☯️`" == "/☯️:/🍕:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 0 /☯️`" == "/☯️:/🍕:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 0 /☯️`" == "/☯️:/🍕:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 1 /☯️`" == "/🍕:/☯️:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 1 /☯️`" == "/🍕:/☯️:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 2 /☯️`" == "/🍕:/🦎:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 2 /☯️`" == "/🍕:/🦎:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 3 /☯️`" == "/🍕:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 3 /☯️`" == "/🍕:/🦎" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -t /☯️`" == "/🍕:/🦎:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --tail /☯️`" == "/🍕:/🦎:/☯️" ] || echo "FAILURE"

export PATH=/🍕:
TEST="${PREFIX}/way bytes";([ "`${TEST}`" == "6" ] || echo "FAILURE: ${TEST}")
TEST="${PREFIX}/way chars";([ "`${TEST}`" == "3" ] || echo "FAILURE: ${TEST}")
[ "`${PREFIX}/way count`" == "2" ] || echo "FAILURE"

[ "`${PREFIX}/way get`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 1`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 1`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 2`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 2`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -t`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --tail`" == "" ] || echo "FAILURE"

[ "`${PREFIX}/way delete`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 0`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 0`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -t`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --tail`" == "/🍕" ] || echo "FAILURE"

[ "`${PREFIX}/way insert /☯️`" == "/☯️:/🍕:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 0 /☯️`" == "/☯️:/🍕:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 0 /☯️`" == "/☯️:/🍕:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 1 /☯️`" == "/🍕:/☯️:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 1 /☯️`" == "/🍕:/☯️:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 2 /☯️`" == "/🍕::/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 2 /☯️`" == "/🍕::/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 3 /☯️`" == "/🍕:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 3 /☯️`" == "/🍕:" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -t /☯️`" == "/🍕::/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --tail /☯️`" == "/🍕::/☯️" ] || echo "FAILURE"

export PATH=:/🍕
TEST="${PREFIX}/way bytes";([ "`${TEST}`" == "6" ] || echo "FAILURE: ${TEST}")
TEST="${PREFIX}/way chars";([ "`${TEST}`" == "3" ] || echo "FAILURE: ${TEST}")
[ "`${PREFIX}/way count`" == "2" ] || echo "FAILURE"

[ "`${PREFIX}/way get`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 0`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 0`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 1`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get -i 2`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get --index 2`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way get -t`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way get --tail`" == "/🍕" ] || echo "FAILURE"

[ "`${PREFIX}/way delete`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 0`" == "/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -i 1`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --index 1`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete -t`" == "" ] || echo "FAILURE"
[ "`${PREFIX}/way delete --tail`" == "" ] || echo "FAILURE"

[ "`${PREFIX}/way insert /☯️`" == "/☯️::/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 0 /☯️`" == "/☯️::/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 0 /☯️`" == "/☯️::/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 1 /☯️`" == ":/☯️:/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 1 /☯️`" == ":/☯️:/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 2 /☯️`" == ":/🍕:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 2 /☯️`" == ":/🍕:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -i 3 /☯️`" == ":/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --index 3 /☯️`" == ":/🍕" ] || echo "FAILURE"
[ "`${PREFIX}/way insert -t /☯️`" == ":/🍕:/☯️" ] || echo "FAILURE"
[ "`${PREFIX}/way insert --tail /☯️`" == ":/🍕:/☯️" ] || echo "FAILURE"

