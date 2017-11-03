Table table;

void setup()
{
  table = loadTable("FILE.CSV", "header");
  println(table.getRowCount()+ "total rows in table");
  
  for(TableRow row: table.rows())
  {
    String date = row.getString("date");
    String time = row.getString("time");
    String latitude = row.getString("lat");
    String longitude = row.getString("loc");
    println(date+"--"+time+"--" +latitude+ "--"+longitude);
  }
}