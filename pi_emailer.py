import sys
import os
import smtplib
import imaplib
import email
from email.mime.text import MIMEText
import pickle
from time import sleep
from random import *

MY_EMAIL = 'purdue.ece.477.team.11.f17@gmail.com'
MY_PSWD = 'meshnetwork11'

ENABLE_SUBJECT = 'Fire Notifications - On'
DISABLE_SUBJECT = 'Fire Notifications - Off'


LIST_FILE = '/home/pi/Documents/477grp11/Raspberry-Pi-Email-System/email_list.p'
#LIST_FILE = 'email_list.p'


ENABLED = 'ENABLED'
DISABLED = 'DISABLED'

BASE_MSG = 'Hello,\n\n This is an automated email from the Purdue ECE 477 Team 11 fire detection sensor network.\n'
TECH_MSG = 'This message has been forward to you for support help\n\n'

JAY = 0
ALINE = 1
SHEF = 2
MEGAN = 3


class pi_emailer():
	def __init__(self, email, pswd):
		self.email = email
		self.pswd = pswd
		self.list_file = LIST_FILE
		self.base_msg = BASE_MSG
		self.enable_subject = ENABLE_SUBJECT
		self.disable_subject = DISABLE_SUBJECT

		self.tech_support = ["kjknepp@purdue.edu", "aesquena@purdue.edu", "chef@purdue.edu", "hedgesm@purdue.edu", "hedgesm@purdue.edu"]

		if(os.path.isfile(LIST_FILE)):
			self.email_list = pickle.load(open(LIST_FILE,'rb'))
		else:
			self.email_list = dict()
		#print self.email_list

	def read_emails(self):
		ret = False
		mail = imaplib.IMAP4_SSL('imap.gmail.com')
		mail.login(self.email,self.pswd)
		mail.list()
		mail.select('inbox')
	
		(retcode, messages) = mail.search(None,'(UNSEEN)')
		if retcode == 'OK':
			for id in messages[0].split():
				typ, data = mail.fetch(id,'(RFC822)')
				for response_part in data:
					if isinstance(response_part,tuple):
						original = email.message_from_string(response_part[1])
						if original['Subject'] == self.enable_subject:
							address = original['From'].split('<')[1].split('>')[0]
							self.email_list[address] = ENABLED
						elif original['Subject'] == self.disable_subject:
							address = original['From'].split('<')[1].split('>')[0]
							self.email_list[address] = DISABLED
						else: #Forward Message to Customer support
							s = smtplib.SMTP(host='smtp.gmail.com', port=587)
							s.ehlo()
							s.starttls()
							s.login(self.email,self.pswd)


							if original.is_multipart():
								for part in original.walk():
									ctype = part.get_content_type()
									cdispo = str(part.get('Content-Disposition'))

									# skip any text/plain (txt) attachments
									if ctype == 'text/plain' and 'attachment' not in cdispo:
										body = part.get_payload(decode=True)  # decode
										break
							else:
								body =  original.get_payload(decode=True)


							if original['Subject'] == 'Base SSH - Help':
								self.send_email(s, self.tech_support[JAY], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == 'Custom Website Setup - Help':
								self.send_email(s, self.tech_support[ALINE], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == "Android Device can't Find Node":
								self.send_email(s, self.tech_support[SHEF], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == "Base can't Access Network":
								self.send_email(s, self.tech_support[JAY], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == 'Android Application Issues':
								self.send_email(s, self.tech_support[ALINE], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == 'Can Access Node, Cannot get Data':
								self.send_email(s, self.tech_support[MEGAN], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == "Node Within Range, Still can't Access": 
								self.send_email(s, self.tech_support[SHEF], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)
							elif original['Subject'] == "System Halt":
								ret = True
							else:
								#Choose Random recipient from tech support
								#Since Megan only fields one type of specific problem,
								#she gets a 40% chance whereas everyone else gets a 20% chance
								self.send_email(s, self.tech_support[randint(0,4)], "FWD: " + original['Subject'], TECH_MSG + "From: " + original['From'] + "\n\n" + body)



							s.quit()
		return ret




	def send_updates(self, subject, message):
		s = smtplib.SMTP(host='smtp.gmail.com', port=587)
		s.ehlo()
		s.starttls()
		s.login(self.email,self.pswd)
		for key in self.email_list.keys():
			if(self.email_list[key] == ENABLED):
				self.send_email(s, key, subject, message)
		s.quit()



	def send_email(self, server, to_addr, subject, message):
		msg = MIMEText(self.base_msg + message)
		msg['Subject'] = subject
		msg['From'] = self.email
		msg['To'] = to_addr
		
		server.sendmail(self.email,to_addr,msg.as_string())

	def save_list(self):
		pickle.dump(self.email_list, open(self.list_file,'wb'))



if __name__=='__main__':
	emailer = pi_emailer(MY_EMAIL, MY_PSWD)
	thing = emailer.read_emails()
	emailer.send_updates("FIRE!!!!!!", "\nWe're all going to die!!!!")
	emailer.save_list()
