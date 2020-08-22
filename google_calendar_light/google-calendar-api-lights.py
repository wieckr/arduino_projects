#!/usr/bin/python -OO
# Copyright 2018 Rob Wieck
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

# This program will read from a Google Calendar to determine if the light action should be on.
# Any Agenda item (one, or two, or more) during the current time (using the google freebusy API) will mean "lights on".

#INSTALLATION
# 1) pip install --upgrade google-api-python-client
# 2) follow the instructions to setup the client_secret (step 1) at https://developers.google.com/google-apps/calendar/quickstart/python
# 3) setup crontab to run the script every minute
# 4) if using arduino, use the code to load to the arduino

from __future__ import print_function
import httplib2
import os
import collections
from apiclient import discovery
from oauth2client import client
from oauth2client import tools
from oauth2client.file import Storage
import datetime
from datetime import timedelta

try:
    import argparse
    flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
except ImportError:
    flags = None
from time import gmtime, strftime

LEDMODE = 'arduino' #change to 'gpio' if using rpi's GPIO ports

#if you are using an arduino over serial to control the light function, keep this, otherwise enable the GPIO functions for 3 separate red, blue and yellow LEDs
if (LEDMODE == 'arduino'):
    import serial
    arduinoSerialData = serial.Serial('/dev/ttyUSB0', 9600)
else:
    import RPi.GPIO as GPIO
    YELLOWPIN = 13
    BLUEPIN = 19
    REDPIN = 26



# If modifying these scopes, delete your previously saved credentials
# at ~/.credentials/calendar-python-quickstart.json
SCOPES = 'https://www.googleapis.com/auth/calendar.readonly'
CLIENT_SECRET_FILE = 'client_secret.json'
APPLICATION_NAME = 'Google Calendar API Python Light Control'

NOW = datetime.datetime.utcnow() - timedelta(minutes=1) #the google freebusy api needs to have 2 times, so NOW is really a minute ago

def get_credentials():
    """Gets valid user credentials from storage.

    If nothing has been stored, or if the stored credentials are invalid,
    the OAuth2 flow is completed to obtain the new credentials.

    Returns:
        Credentials, the obtained credential.
    """
    home_dir = os.path.expanduser('~')
    credential_dir = os.path.join(home_dir, '.credentials')
    if not os.path.exists(credential_dir):
        os.makedirs(credential_dir)
    credential_path = os.path.join(credential_dir,
                                   'calendar-python-quickstart.json')

    store = Storage(credential_path)
    credentials = store.get()
    if not credentials or credentials.invalid:
        flow = client.flow_from_clientsecrets(CLIENT_SECRET_FILE, SCOPES)
        flow.user_agent = APPLICATION_NAME
        if flags:
            credentials = tools.run_flow(flow, store, flags)
        else: # Needed only for compatibility with Python 2.6
            credentials = tools.run(flow, store)
        print('Storing credentials to ' + credential_path)
    return credentials

def get_upcoming_events():
    """
    Creates a Google Calendar API service object and outputs a list of the next
    10 events on the user's calendar.
    """
    now = NOW.isoformat() + 'Z'
    credentials = get_credentials()
    http = credentials.authorize(httplib2.Http())
    service = discovery.build('calendar', 'v3', http=http)
    
    print('Getting the upcoming 10 events')
    eventsResult = service.events().list(
        calendarId='primary', timeMin=now, maxResults=10, singleEvents=True,
        orderBy='startTime').execute()
    events = eventsResult.get('items', [])
    all_event_info = {}

    if not events:
        print('No upcoming events found.')
        return (false)
    for event in events:
        start = event['start'].get('dateTime', event['start'].get('date'))
        all_event_info[start] = event['summary']
    all_event_info_sorted = collections.OrderedDict(sorted(all_event_info.items()))
    return (all_event_info_sorted)
    

def main():
    """
    Creates a Google Calendar API service object and outputs a list of the next
    10 events on the user's calendar.
    """
    oneMin = (NOW + timedelta(minutes=1)).isoformat() + 'Z'
    twoMin = (NOW + timedelta(minutes=2)).isoformat() + 'Z'
    tenMin = (NOW + timedelta(minutes=10)).isoformat() + 'Z'
    now = NOW.isoformat() + 'Z'
    
    upcoming_events = get_upcoming_events()
    for times, events in upcoming_events.iteritems():
        print (times, '\t\t' ,events)
    """
    Uses the freebusy API to determine if your calendar has a current item (or one within 10 mins from now)
    """
    
    print('Checking if busy')
    if (LEDMODE == 'gpio'):
        setupPins()
    busyNow = checkIfBusy(now, oneMin)
    busyTen = checkIfBusy(now, tenMin)
    busyOne = checkIfBusy(now, twoMin)
    if (busyNow == True):
        turnLightsOn()
    elif ((busyTen == True) and (busyOne == False)): #10 min warning
        turnEarlyWarning()
    elif (busyOne == True): #1 min warning
        turnLateWarning()
    else:
        turnLightsOff()



def checkIfBusy(start,end):
    body = {
        "timeMin": start,
        "timeMax": end,
        "timezone": 'US/Central',
        "items": [{"id": 'youremail@gmail.com'}]  #replace with the calendar email address
    }
    credentials = get_credentials()
    http = credentials.authorize(httplib2.Http())
    service = discovery.build('calendar', 'v3', http=http)
    eventsBusy = service.freebusy().query(body=body).execute()
    cal_dict = eventsBusy[u'calendars']
    print 
    for cal_name in cal_dict:
        cal_busy = cal_dict[cal_name]
        for cal_busy_name in cal_busy:
            times = cal_busy[cal_busy_name]
            if times == []:
                print ("Horray! No Meetings!")            
                turnLightsOff()
                return False
            else:
                print ("Dangit, a meeting is happening!")                
                for endpoints in times:
                    start_busy = endpoints[u'start']
                    end_busy = endpoints[u'end']
                    print ("I am done being busy at:", end_busy)
                return True

def turnEarlyWarning():
        if (LEDMODE == 'arduino'):
            arduinoSerialData.write('2')
        else:        
            GPIO.output(BLUEPIN, GPIO.LOW)
            GPIO.output(YELLOWPIN, GPIO.HIGH)
            GPIO.output(REDPIN, GPIO.LOW)
        
def turnLateWarning():
        if (LEDMODE == 'arduino'):
            arduinoSerialData.write('3')
        else:
            GPIO.output(BLUEPIN, GPIO.LOW)
            GPIO.output(YELLOWPIN, GPIO.LOW)
            GPIO.output(REDPIN, GPIO.HIGH)
        
def turnLightsOn():
        if (LEDMODE == 'arduino'):
            arduinoSerialData.write('1')
        else:
            GPIO.output(BLUEPIN, GPIO.HIGH)
            GPIO.output(YELLOWPIN, GPIO.LOW)
            GPIO.output(REDPIN, GPIO.LOW)

def turnLightsOff():
        if (LEDMODE == 'arduino'):
            arduinoSerialData.write('0')
        else:
            GPIO.output(BLUEPIN, GPIO.LOW)
            GPIO.output(YELLOWPIN, GPIO.LOW)
            GPIO.output(REDPIN, GPIO.LOW)

def setupPins():
      	GPIO.setmode(GPIO.BCM) # Needed in GPIO version 0.3.1
        GPIO.setwarnings(False)
  	GPIO.setup(BLUEPIN, GPIO.OUT)
 	GPIO.setup(REDPIN, GPIO.OUT)
 	GPIO.setup(YELLOWPIN, GPIO.OUT)

 	
'''
import os, sys

if os.popen('whoami').readline().strip() == 'root' :
 	print "OK, you're root"
 	root = True
else:
 	print "\nAttention: please run as root with 'sudo'. This scripts GPIO which need root-rights."
 	print "Now exiting ..."
 	sys.exit(1)
 	root = False

'''

if __name__ == '__main__':
    main()
