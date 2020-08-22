#!/usr/bin/python -OO
# Copyright 2012 Sander Jonkers
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

# This program will read from a Google Calendar to determine if the light should be on.
# Any Agenda item (one, or two, or more) during the current time will mean "lights on".
# No Agenda item means "lights off"


from time import gmtime, strftime

import serial
arduinoSerialData = serial.Serial('/dev/ttyUSB0', 9600)
# arduinoSerialData.write('0'); 
# Useful: http://strftime.net/

# The Google Calendar calendar ics items are in GMT / Zulu time. So first find that time for this system, in the same string
# format as the Google calendar ics 

currenttimeZulu = strftime("%Y%m%dT%H%M%SZ", gmtime())
print "\nCurrent time in GMT / Zulu time is:",currenttimeZulu
# Gives: 20120715T114225Z


# the secret (yes!) URL for the agenda. See http://support.google.com/calendar/bin/answer.py?hl=en&answer=37648 how to find your URL
iCalAgendaUrl = ' https://calendar.google.com/calendar/ical/{yoursecreturl}/basic.ics'


'''
The interesting lines in the basic.ics

BEGIN:VEVENT
DTSTART:20120715T130000Z
DTEND:20120715T140000Z
SUMMARY:middagverlichting
END:VEVENT
'''


import urllib2

req = urllib2.Request(iCalAgendaUrl)
response = urllib2.urlopen(req)
print "FYI: Agenda retrieved"
# the_page = response.read()
# print the_page

LightsOn = False # Default is lights off

print "Start parsing the Agenda"
for thisline in response.readlines():
 # print thisline
	regel = thisline.rstrip()
 	if regel.startswith('BEGIN:VEVENT'):
 	 # print "BEGIN:VEVENT gevonden!!!", regel
  		starttime = None
  		endtime = None
 	 	summary = None
 	if regel.startswith('DTSTART'):
  	# print "DTSTART gevonden!!!", regel
  		starttime = regel.split(':')[1]
  		# print starttime
 	if regel.startswith('DTEND'):
  		# print "DTEND gevonden!!!", regel
  		endtime = regel.split(':')[1]
  		# print endtime
 	if regel.startswith('SUMMARY'):
  		# print "DTSTART gevonden!!!", regel
  		summary = regel.split(':')[1]
  		# print starttime

  	if regel.startswith('END:VEVENT'):
  		# print "END:VEVENT gevonden!!!", regel
  		if (starttime < currenttimeZulu) and (currenttimeZulu <= endtime):
   			print starttime, endtime, summary
   			print "This event says the light should be on!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
   			LightsOn = True
  		else:
   			dummy = 1
   			# print "*This* event says the light should be off"
   			#Attention: do NOT set LightsOn to False (=Off)!!!!


	#if LightsOn:
 		#print "Lights should be on"
	#else:
 		#print "Lights should be off"

def TurnLightsOn():
	import RPi.GPIO as GPIO

  	GPIO.setmode(GPIO.BCM) # Needed in GPIO version 0.3.1

  	GPIO.setup(19, GPIO.OUT)


  	print("Turn it on: press button ...")
 	GPIO.output(19, True)

  	import time
 	time.sleep(1)

  	#print("... and release button.")
 	#GPIO.output(19, False)
        
        arduinoSerialData.write('1')

def TurnLightsOff():

        arduinoSerialData.write('0')
 	import RPi.GPIO as GPIO

  	GPIO.setmode(GPIO.BCM)
 	GPIO.setup(19, GPIO.OUT)

  	#print("Turn it off: press Off button ...")
 	#GPIO.output(19, True)

  	import time
 	time.sleep(1)

  	print("... and release button.")
 	GPIO.output(19, False)


import os, sys

if os.popen('whoami').readline().strip() == 'root' :
 	print "OK, you're root"
 	root = True
else:
 	print "\nAttention: please run as root with 'sudo'. This scripts GPIO which need root-rights."
 	print "Now exiting ..."
 	sys.exit(1)
 	root = False


if LightsOn:
 	print "Turning Lights On ..."
 	TurnLightsOn()
else:
 	print "Turning Lights Off ..."
 	TurnLightsOff()
	print "Finished"

