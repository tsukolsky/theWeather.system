#!/usr/bin/python
##This script is resposible for alerting the owner of the system, Todd Sukolsky
##when an email has subscribed or unsubscribed from the list..
import os
import sys
import optparse
import smtplib
import socket
from email.mime.text import MIMEText
import datetime

changedEmail=''

use="Usage: %prog [options] <arguments>. Email is input string."
parser=optparse.OptionParser(usage=use)
parser.add_option('-e',dest='emailIn',help='Email that was changed')
(options,args)=parser.parse_args()

if options.emailIn is None:
	print "No email..."
	exit(0)
else:
	changedEmail=options.emailIn

#Open file, get lines to parse
IF=open('/home/sukolsky/Documents/emailList.txt','r')
emails=IF.readlines()
IF.close()

#If the changed email is in the file, then it was added. Otherwise deleted
message=''
for anEmail in emails:
	if changedEmail+'\n'==anEmail:
		message+='Added '

if message=='':
	message+='Deleted '

#Add changed email to the message
message+=changedEmail.rstrip()

#Email Params
to='tsukolsky@gmail.com'
gmail_user='tsukolsky'
gmail_password=''
smtpserver=smtplib.SMTP('smtp.gmail.com',587)
smtpserver.ehlo()
smtpserver.starttls()
smtpserver.ehlo
smtpserver.login(gmail_user,gmail_password)
today=datetime.date.today()

#Create and send message
msg=MIMEText(message)
msg['Subject']='Email List Change on %s' % today.strftime('%b %d %Y')
msg['From']=gmail_user
msg['To']=to
smtpserver.sendmail(gmail_user,[to],msg.as_string())
smtpserver.quit()
exit()
