#!/bin/bash

#Install aws-cli in ec2 (sudo apt install awscli)
#Get Non MFA account access and secret keys and configure in aws cli

#Example:
#$ aws configure
#AWS Access Key ID [None]: AKIAIOSFODNN7EXAMPLE
#AWS Secret Access Key [None]: wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY
#Default region name [None]: us-west-1
#Default output format [None]: json

echo "Starting backup upload to AWS S3"
cd /usr/local/dynomitedb/home/backup/
file=$(ls -t dump-* | head -1) 
#Gets the latest file from /usr/local/dynomitedb/home/backup/ directory sorted by time
filepath=/usr/local/dynomitedb/home/backup/$file
echo "uploading $filepath to s3"
currentdate=$(date '+%d-%m-%Y-%H:%M:%S')
hostname=$(hostname -i)
sudo aws s3 cp ./$file s3://dynomite-backup-1/ --metadata '{"date":'"\"$currentdate\""', "hostname":'"\"$hostname\""'}' --grants read=uri=http://acs.amazonaws.com/groups/global/AllUsers

returnvalue=$?
echo "AWS S3 upload return value $returnvalue"
if [ $returnvalue -ne 0 ]; then
    echo "Upload Failed. Return code was not zero but $returnvalue"
fi