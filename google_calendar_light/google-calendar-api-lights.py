#!/usr/bin/python -OO
# Copyright 2017 Rob Wieck
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
# Any Agenda item (one, or two, or more) during the current time (using the google freebusy API)will mean "lights on".
# No Agenda item means "lights off"



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

#if you are using an arduino over serial to control the light function, keep this, otherwise enable the GPIO functions
import serial
arduinoSerialData = serial.Serial('/dev/ttyUSB0', 9600)
#import RPi.GPIO as GPIO
#GPIOPIN = 19


# If modifying these scopes, delete your previously saved credentials
# at ~/.credentials/calendar-python-quickstart.json
SCOPES = 'https://www.googleapis.com/auth/calendar.readonly'
CLIENT_SECRET_FILE = 'client_secret.json'
APPLICATION_NAME = 'Google Calendar API Python Quickstart'
NOW = datetime.datetime.utcnow().isoformat() + 'Z' # 'Z' indicates UTC time
ONEHOUR = (datetime.datetime.utcnow() + timedelta(hours=1)).isoformat() + 'Z'


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
    credentials = get_credentials()
    http = credentials.authorize(httplib2.Http())
    service = discovery.build('calendar', 'v3', http=http)
    
    print('Getting the upcoming 10 events')
    eventsResult = service.events().list(
        calendarId='primary', timeMin=NOW, maxResults=10, singleEvents=True,
        orderBy='startTime').execute()
    events = eventsResult.get('items', [])
    all_event_info = {}



    if not events:
        print('No upcoming events found.')
        return (false)
    for event in events:
        start = event['start'].get('dateTime', event['start'].get('date'))

        all_event_info[start] = event['summary']
        #all_event_info.append(event['summary'])
        #print(start, event['summary'])
    all_event_info_sorted = collections.OrderedDict(sorted(all_event_info.items()))
    return (all_event_info_sorted)
    

def main():
    """Shows basic usage of the Google Calendar API.

    Creates a Google Calendar API service object and outputs a list of the next
    10 events on the user's calendar.
    """
    upcoming_events = get_upcoming_events()
    for times, events in upcoming_events.iteritems():
        print (times, '\t\t' ,events)
    
    print('Checking if busy')
    body = {
        "timeMin": NOW,
        "timeMax": ONEHOUR,
        "timezone": 'US/Central',
        "items": [{"id": 'rwieck@nd.edu'}]
    }
    credentials = get_credentials()
    http = credentials.authorize(httplib2.Http())
    service = discovery.build('calendar', 'v3', http=http)
    eventsBusy = service.freebusy().query(body=body).execute()
    cal_dict = eventsBusy[u'calendars']

    for cal_name in cal_dict:
        cal_busy = cal_dict[cal_name]
        for cal_busy_name in cal_busy:
            times = cal_busy[cal_busy_name]
            if times == []:
                print ("Horray! No Meetings!")
                turnLightsOff()
            else:
                print ("Dangit, a meeting is happening!")
                turnLightsOn()
                for endpoints in times:
                    start_busy = endpoints[u'start']
                    end_busy = endpoints[u'end']
                    print ("I am done being busy at:", end_busy)
                    
                    
def turnLightsOn():


  	#GPIO.setmode(GPIO.BCM) # Needed in GPIO version 0.3.1
  	#GPIO.setup(GPIOPIN, GPIO.OUT)
  	#print("Turn it on: press button ...")
 	#GPIO.output(GPIOPIN, True)

  	#import time
 	#time.sleep(1)

  	#print("... and release button.")
 	#GPIO.output(19, False)
        
        arduinoSerialData.write('1')

def turnLightsOff():

        arduinoSerialData.write('0')
  	#GPIO.setmode(GPIO.BCM)
 	#GPIO.setup(GPIOPIN, GPIO.OUT)

  	#print("Turn it off: press Off button ...")
 	#GPIO.output(19, True)

  	#import time
 	#time.sleep(1)

  	#print("... and release button.")
 	#GPIO.output(GPIOPIN, False)

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