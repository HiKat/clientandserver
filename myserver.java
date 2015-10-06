import java.net.*;
import java.io.*;
import java.util.regex.*;

class Server {
    public static void main(String[] args){
	if(args.length != 1) return;
	try {
	    ServerSocket serverS = new ServerSocket(Integer.parseInt(args[0]));
	    while (true) {
		new ServerThread(serverS.accept()).start();
		System.out.println("New connection.");
	    }
	} 
	catch(IOException e){
	    System.out.println("IO exception.");
	    System.exit(1);
	}
    }
}
class ServerThread extends Thread {
    Socket clientS;

    public ServerThread(Socket acceptedS) {
	clientS = acceptedS;
    }

    public void run(){
	try{
	    PrintStream out = new PrintStream(clientS.getOutputStream(), true);
	    BufferedReader in = new BufferedReader(new InputStreamReader(clientS.getInputStream()));
	    // receive requests from client (until an empty line)
	    /*example 
	      GET /index.html HTTP/1.1[CRLF]   
	      Host: www.hogehoge.com:80[CRLF]
	    */
	    String[] requestLines = new String[2];
	    for(int n = 0;n < 2;n++){
		requestLines[n] = in.readLine();
	    }

	    //analyze request from client
	    Pattern pat = Pattern.compile("GET (.+) HTTP/1.1");
	    Matcher mat = pat.matcher(requestLines[0]);
	    mat.find();
	    String filePath = "."+mat.group(1);//convert mat.group(1) into relative path.

	    // send contents to client
	    String FileName = filePath; 
	    String Line; 
	    BufferedReader bf = new BufferedReader(new FileReader(FileName));
	    while((Line = bf.readLine()) != null){
		out.println(Line);
	    }

	    out.close();
	    in.close();
	    clientS.close();
	} 
	catch(FileNotFoundException e){
	    System.out.println(e);
	}
	catch(IOException e){
	    System.out.println("IO exception.");
	    System.out.println(e);
	    System.exit(1);
	}
    }
}
