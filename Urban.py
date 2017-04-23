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

Pump = "100"
Valve = "200"

@app.route("/data/<n>/<Water>")																			#Get data From Client(Wemos)
def data(n,Water):
	global Pump 
	global Valve
	log= str(datetime.now()) + "||Value: %s Water %s"%(n,Water)
	print log
	socketio.emit('status',Water)
	return "%s,%s"%(Pump,Valve)

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
	if(ComP == 2):
		Pump = "100"
	if (ComV == 1):
		Valve = "201"
	if(ComV == 2):
		Valve = "200"


		

@app.route("/admin")
def ad():
	return render_template('UrbanControl.html')


