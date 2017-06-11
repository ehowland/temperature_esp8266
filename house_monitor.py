"""A basic (single function) API written using hug"""
import hug
import sqlite3
import sys, time, datetime, os



@hug.cli()
@hug.get('/add_reading')
def add_temp_reading(location: hug.types.text="error, no location", temp_c: hug.types.float_number=0.0, huzid: hug.types.number=0):
    """adds some data to the database"""
    global cur, db
    cur = open_db()
    temp_f = temp_c * 9.0 / 5.0 + 32.0
    timestamp= int(time.time())  #time in the epoch from system gmt verson
    print(timestamp);  #ex. 1480391479 i.e. 
    
    """
        cur.execute("CREATE TABLE observations("\
          "ob_id INTEGER PRIMARY KEY,"\
          "location TEXT,"\
          "temp_f real,"\
          "huzid INTEGER,"\
          "ob_timestamp TEXT);")
    """
    
    cur.execute("INSERT INTO {tn} (ob_id, location, temp_f, huzid, ob_timestamp)"\
                " VALUES(NULL, {location}, {temp_f}, {huzid}, (CURRENT_TIMESTAMP))"\
                .format(tn="observations", location=location,  temp_f=temp_f, huzid=huzid ));
    db.commit();
    db.close();
    return "Success for {location} temp: {temp_f} huzid: {huzid}".format(location=location,  temp_f=temp_f, huzid=huzid )
    


def analyse_locations(rows):
  
  return result


@hug.get('/list_readings', output=hug.output_format.html)
def recent_temps():
    """get a list of temperatures"""
    cur = open_db();
    cur.execute("SELECT * FROM observations ORDER BY location ASC, ob_timestamp ASC")
    result = "";
    observations = cur.fetchall();
    #print(observations);
    for obs in observations:
      result += "<p>"+str(obs)+"</p>"
      
    print(result);
    return result

def settup_db():
  """setup structure of database """
  cur=open_db()
  """
  INTEGER: A signed integer up to 8 bytes depending on the magnitude of the value.
  REAL: An 8-byte floating point value.
  TEXT: A text string, typically UTF-8 encoded (depending on the database encoding).
  BLOB: A blob of data (binary large object) for storing binary data.
  NULL: A NULL value, represents missing data or an empty cell.
      """

  cur.execute("CREATE TABLE observations("+
   "ob_id INTEGER PRIMARY KEY,"+
   "huzid INTEGER,"+
   "location TEXT,"+
   "temp_f real,"+
   "ob_timestamp TEXT);")
  

  db.commit()
  db.close()
  return


def open_db():
  global db, cur
  try:
    db = sqlite3.connect("observations.db")
    cur = db.cursor()    
    #cur.execute('SELECT SQLITE_VERSION()')
    #data = cur.fetchone()
    #print ("SQLite version: %s" % data)
  except Exception as e:
      print ("Error !%s!" % e.args[0])
      sys.exit(1);
  return cur

######===========  MAIN ================   

##### test if database exists, create one if it does not. 
try:
  statinfo = os.stat("observations.db")
  print("statinfor is:", statinfo)
except:
  settup_db()


if __name__ == '__main__':
    recent_temps.interface.cli()
    list_readings.interface.cli()
