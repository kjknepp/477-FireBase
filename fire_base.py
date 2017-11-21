import threading
from firebase import firebase
from pi_emailer import pi_emailer
from ctypes import *
from time import sleep

PSWD = 'meshnetwork11'
USR = 'purdue.ece.477.team.11.f17@gmail.com'

FIREBASE_SECRET = 'eC2fJIDZZtcnugTgmw64o1ejbvQbUBXdbmwQBJkv'
FIREBASE_URL = 'https://firenode-e06f4.firebaseio.com/'

THRESHOLD = 40

EMAIL_SUBJECT = 'ATTN: Fire Detected'

class db_msg():
	def __init__(self, hash, rxid, txid, opcode, data, temperature, humidity, pressure):
		self.hash = hash
		self.rxid = rxid
		self.txid = txid
		self.opcode = opcode
		self.data = data
		self.temperature = temperature
		self.humidity = humidity
		self.pressure = pressure
		


class FireBase():
	def __init__(self, url, usr, pswd, secret=None, demo_mode=True):
		self.url = url
		self.usr = usr
		self.pswd = pswd
		self.secret = secret
		self.demo_mode = demo_mode

		self.base = firebase.FirebaseApplication(self.url, self.secret)

		self.emailer = pi_emailer(self.usr, self.pswd)
		self.email_thread = threading.Thread(target=self.run_email_thread)
		self.usb_thread = threading.Thread(target=self.run_usb_receiver)
		self.main_thread = threading.Thread(target=self.run)


		self.email_update_list = []
		self.update_lock = False

		self.usb_msg_buffer = []

		self.usb_driver = CDLL('./static_link')
		res_int = self.usb_driver.c_init()
		self.usb_lock = False

		self.db_msg_buffer = []


	def run_email_thread(self):
		while(self.running):
			sleep(30)
			do_stop = self.emailer.read_emails()
			if(do_stop):
				self.running = False
			if(self.email_update_dict.len() > 0):
				self.update_lock = True
				for val in self.email_update_list:
					self.emailer.send_updates(EMAIL_SUBJECT, val)
				self.email_update_list = []
				self.update_lock = false
		self.emailer.save_list()

	def run_usb_receiver(self):		
		while(self.running):
			sleep(0.5); #Give Python room to deal with threads
			if not self.usb_lock:
				self.usb_lock = True
				self.usb_driver.usb_receive()
				hash = self.usb_driver.gethash()
				rxid = self.usb_driver.getrxid()				
				txid = self.usb_driver.gettxid()				
				opcode = self.usb_driver.getopcode()				
				data = self.usb_driver.getdata()				
				temperature = self.usb_driver.gettemperature()
				humidity = self.usb_driver.gethumidity()				
				pressure = self.usb_driver.getpressure()

				msg = db_msg(hash,rxid,txid,opcode,data,temperature,humidity,pressure)
				self.db_msg_buffer.append(msg)

				if(temperature > 1000000):
					if(self.update_lock):
						val = "WE'RE ALL GOING TO DIE!!!!\n\nNode Temperature Reading: " + str(temperature) + "\nNode Humidity Reading: " + str(humidity) + "\nNode Pressure Reading: " + str(pressure)
						while(self.update_lock):
							sleep(0.05)
						self.email_update_list.append(val)
	

				self.usb_lock = False
			


	def run_async(self):
		self.main_thread.start()

	def run(self):
		self.running = True
		self.email_thread.start()
		self.usb_thread.start()

		while(self.running):
			sleep(0.05)
			if(self.demo_mode):
				if self.db_msg_buffer:
					while self.db_msg_buffer:
						sleep(0.05)
						msg = self.db_msg_buffer.pop()
						self.base.put(self.url, '/Nodes', str(msg.txid))
						self.base.put(self.url, '/Nodes/' + str(msg.txid) + '/temp', str(msg.temperature))
						self.base.put(self.url, '/Nodes/' + str(msg.txid) + '/hum', str(msg.humidity))
						self.base.put(self.url, '/Nodes/' + str(msg.txid) + '/press', str(msg.pressure))
						#self.base.put(self.url, '/Nodes/' + str(msg.txid) + '/lat', str(msg.latitude))
						#self.base.put(self.url, '/Nodes/' + str(msg.txid) + '/lon', str(msg.longitude))
			else:
				pass

		self.usb_driver.c_close()




if __name__=='__main__':
	base = FireBase(FIREBASE_URL, USR, PSWD, FIREBASE_SECRET)
	base.run()

	#print result
