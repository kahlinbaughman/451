import java.net.*;
import java.io.*;

class Client {
  public static void main(String[] args) {

    try {

      Socket socket = new Socket("127.0.0.1",Integer.parseInt(args[0]));
      System.out.println("Client has been started at port: " + Integer.parseInt(args[0]));

      System.out.println("Receiving File...");
      System.out.println();
      InputStream is = socket.getInputStream();
      InputStreamReader isr = new InputStreamReader(is);
      BufferedReader br = new BufferedReader(isr);

      String message;

      while((message = br.readLine()) != null)
      {
        System.out.println(message);
      }

      System.out.println();

      System.out.println("File received.");
      System.out.println("Closing connection.");
      socket.close();

    } catch (Exception e) {}
  }
}
