#!/usr/bin/python

import os
import sys
import optparse
import subprocess

email=''

use="Usage: %prog [options] <arguments>. Email is input string."
parser=optparse.OptionParser(usage=use)
parser.add_option('-e',dest='emailIn',help='Email to add delete')

(options,args)=parser.parse_args()

if options.emailIn is None:
	exit(1)
else:
	email=options.emailIn.rstrip().strip()

IF=open('/home/sukolsky/Documents/emailList.txt','r')
emails=IF.readlines()
IF.close()
OF=open('/home/sukolsky/Documents/emailList.txt','w')
hadToDelete=False
changedEmails=''
for anEmail in emails:
	if anEmail!=email+'\n':
		##Write the email back to the file
		OF.write(anEmail)
	else:
		hadToDelete=True
		changedEmails+=anEmail.rstrip()

##If we didn't delete an email we should add the email, then call CommScript
if hadToDelete is False:
	OF.write(email+'\n')
	OF.close()
	changedEmails+=email
	p=subprocess.Popen(['/home/sukolsky/Documents/CommAVR.py','-s','STATS.','-e',email],stdout=subprocess.PIPE)
	out,err=p.communicate()
	try:
		print "Output:"+out
	except:
		print "No output"
	try:
		print "Error:"+err
	except:
		print "No errors"

#make sure the file is closed for the update process
try:
	OF.close()
except:
	print 'File already closed'

##Emails are now done, update owner of change
alert=subprocess.Popen(['/home/sukolsky/Documents/ChangeInEmail.py','-e',changedEmails],stdout=subprocess.PIPE)
aOut,aErr=alert.communicate()
try:
	print "Alerted:"+aOut
except:
	print "No alert output"
try:
	print "Alerted Errors:"+aErr
except:
	print "No alert errors"

##Updated owner, now exit.
exit(0)
