Table old_table;
Table new_table;

void setup()
{
  
  //load original file.csv file 
  old_table = loadTable("FILE.CSV", "header");
  
  //make new table to save data
  new_table = new Table();
  new_table.addColumn("date");
  new_table.addColumn("time");
  new_table.addColumn("lat");
  new_table.addColumn("loc");
  
  //println(old_table.getRowCount()+ "total rows in table");
  
  for(TableRow row: old_table.rows())
  {
 
    String date = row.getString("date");
    String time = row.getString("time");
    String latitude = row.getString("lat");
    String longitude = row.getString("loc");
   // println(date+"--"+time+"--" +latitude+ "--"+longitude);
    
    char[] old_lat_array = latitude.toCharArray();
    char[] old_loc_array = longitude.toCharArray();
    
    char[] new_lat_array = {0,0,0,0,0,0,0,0};
    char[] new_loc_array = {0,0,0,0,0,0,0,0,0};
    
    
    //convert old lat_arry 3738.27 to 37 38.27
    new_lat_array[7] = old_lat_array[6];
    new_lat_array[6] = old_lat_array[5];
    new_lat_array[5] = old_lat_array[4];
    new_lat_array[4] = old_lat_array[3];
    new_lat_array[3] = old_lat_array[2];
    new_lat_array[2] = ' ';
    new_lat_array[1] = old_lat_array[1];
    new_lat_array[0] = old_lat_array[0];
    
     //convert old loc_arry 12712.76 to 127 12.76
    new_loc_array[8] = old_loc_array[7];
    new_loc_array[7] = old_loc_array[6];
    new_loc_array[6] = old_loc_array[5];
    new_loc_array[5] = old_loc_array[4];
    new_loc_array[4] = old_loc_array[3];
    new_loc_array[3] = ' ';
    new_loc_array[2] = old_loc_array[2];
    new_loc_array[1] = old_loc_array[1];
    new_loc_array[0] = old_loc_array[0];
    
    String final_lat = "";
    String final_loc = "";
    
    int lengthOfLat = new_lat_array.length;
    int lengthOfLoc = new_loc_array.length;
    
    //convert char[] type to String type to setString 
    for(int i=0; i<lengthOfLat; i++)
    {
      final_lat += str(new_lat_array[i]);
    }
    for(int j=0; j<lengthOfLoc; j++)
    {
      final_loc += str(new_loc_array[j]);
    }
   
   TableRow newRow = new_table.addRow();
   newRow.setString("date", date);
   newRow.setString("time", time);
   newRow.setString("lat", final_lat);
   newRow.setString("loc", final_loc);
   saveTable(new_table, "file_new.csv");
  }
  println("succefully convert data and save it to new file");
}