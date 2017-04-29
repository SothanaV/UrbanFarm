from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, send, emit
from datetime import datetime,timedelta
import json
from StringIO import StringIO
import sqlite3

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)
##Defind Start Value##
io = StringIO()

global Pump
global Value
global Mode

Pump = "100"
Valve = "200"

@app.route("/data/<t>/<h>/<w>")																			#Get data From Client(Wemos)
def data(t,h,w):
	global Pump 
	global Valve
	global Mode
	log= str(datetime.now()) + "|| Temp %s Humi %s W_Level %s"%(t,h,w)
	print log
	socketio.emit('status',w)
	socketio.emit('s2c_t',t)
	socketio.emit('s2c_h',h)
	return "%s,%s,%s"%(Pump,Valve,Mode)

@socketio.on('c2s')																				#listen Data From Browser parth socketio "c2s" = cilent to server 
def C2S(data):
	global Pump
	global Valve

	sdata=json.loads(data)
	ComV = int(sdata['V'])
	ComP = int(sdata['P'])
	print "Pump %s"%(ComP)
	print "ComV %s"%(ComV)

	if (ComP == 1):
		Pump = "101"
		print "Pump_ON"
	if(ComP == 2):
		Pump = "100"
		print "Pump_OFF"
	if (ComV == 1):
		Valve = "201"
		print "Valve_ON"
	if(ComV == 2):
		Valve = "200"
		print "Valve_OFF"
@socketio.on('c2sClick')
def Mode(command):
	global Mode
	if(command==1):
		Mode = "A"
	if(command==0):
		Mode = "M"
	print "Command %s Mode %s"%(command,Mode)	
@app.route("/admin")
def ad():
	return render_template('UrbanControl.html')
@app.route("/switch")
def switch():
	return render_template('switch.html')
 

