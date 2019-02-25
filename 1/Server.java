import java.net.*;
import java.io.*;

class Server
{
  public static void main(String[] args)
  {
    File lotrScript = new File("serverfile.txt");

    try
    {

      ServerSocket server = new ServerSocket(Integer.parseInt(args[0]));
      System.out.println("Server has been started at port: " + args[0]);

      System.out.println("Waiting for Client....");

      Socket socket = server.accept();
      System.out.println("Client accepted.");
      
      System.out.println("Sending file.");

      BufferedReader br = new BufferedReader(new FileReader(lotrScript));

      OutputStream os = socket.getOutputStream();
      OutputStreamWriter osw = new OutputStreamWriter(os);
      BufferedWriter bw = new BufferedWriter(osw);

      String message;

      while((message = br.readLine()) != null)
      {
          String sendMessage = message + "\n";
          bw.write(sendMessage);
          bw.flush();
      }

      System.out.println("File sent.");

      System.out.println("Closing connection.");
      socket.close();
      bw.close();
    } catch (Exception e) {}
  }
}
