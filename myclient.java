import java.net.*;
import java.io.*;
import java.util.regex.*;
import java.util.Date;

class Client {
    public static void main(String[] args){
	if(args.length != 0) return;
	  while(true){
	  run(prompt());
	  }
    }

    public static String prompt(){
	String st ="";
	try{
	    System.out.print("Enter URL>>");
	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	    st = br.readLine(); 
	}
	catch(IOException e){
	    System.out.println("IO exception.");
	    System.out.println(e);
	    System.exit(1);
	}
	finally{
	    return st;
	}
    }

    public static void run(String st){
	try{
	    Pattern pat = Pattern.compile("http://([^/]+)/(.+)");
	    Matcher mat = pat.matcher(st);
	    mat.find();
	    String hostNameAndPort = mat.group(1);
	    String urlPath = "/"+mat.group(2);
	    String hostName;
	    String portNumber;
	    if(hostNameAndPort.matches(".*"+":"+".*")){
		Pattern pat2 = Pattern.compile("(.*):(.*)");
		Matcher mat2 = pat2.matcher(hostNameAndPort);
		mat2.find();
		hostName = mat2.group(1);
		portNumber = mat2.group(2);
	    }
	    else{
		hostName = hostNameAndPort;
		portNumber = "80";
	    }
	    /////////////////////////////////////	
	    Socket s = new Socket(hostName, Integer.parseInt(portNumber));
	    BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
	    PrintWriter out = new PrintWriter(s.getOutputStream(),true);
	    String buf;

	    // send request to server
	    out.print("GET "+urlPath+" HTTP/1.1\r\n");
	    out.println("Host: "+hostName+":"+portNumber+"\r\n");

	    // receive contents from server
	    //make instance
	    Browse brow = new Browse();
	    ////get current url
	    Pattern pat3 = Pattern.compile("(.*)/([^/]+)");
	    Matcher mat3 = pat3.matcher(st);
	    mat3.find();
	    brow.currentUrl = mat3.group(1)+"/";
		
	    brow.urlPath = urlPath;
	    brow.hostNameAndPort = hostNameAndPort;

	    while(null != (buf = in.readLine())){
		System.out.println(buf);
		brow.collectLinks(buf);
	    }

	    in.close();
	    out.close();
	    s.close();

	    if(brow.linkNumber != 0){
		System.out.println("\n<-----------------Links from this page------------------->");
		for(int m = 0;m < brow.linkNumber;m++){
		    System.out.println("link("+(m+1)+")>> "+brow.listOfLinks[m]);
		}
		while(true){
		    System.out.print("If you want to go to these links, please enter \"y\".If not, please enter \"n\". >>");
		    BufferedReader br2 = new BufferedReader(new InputStreamReader(System.in));
		    String st2 = br2.readLine(); 
		    if(st2.matches("n")){
			break;
		    }
		    if(st2.matches("y")){
			while(true){
			    System.out.print("Enter the link number >>");
			    BufferedReader br3 = new BufferedReader(new InputStreamReader(System.in));
			    String st3 = br3.readLine();
			    int inputNumber = 0;
			    try{
				inputNumber = Integer.parseInt(st3);
			    }
			    catch(NumberFormatException e){
				System.out.println(e);
				System.out.println("Enter number from"+" 1 to "+brow.linkNumber+"!");
			    }
			    if(brow.linkNumber < inputNumber){
				System.out.println("Enter number from"+" 1 to "+brow.linkNumber+"!");
			    }
			    else{
				run(brow.listOfLinks[inputNumber-1]);
				break;
			    }
			}
		    }
		    else{
			System.out.println("Please enter \"y\" or \"n\"");
		    }
		}
	    }
	} 
	catch(UnknownHostException e){
	    System.out.println("Unknown host.");
	    System.out.println(e);
	} 
	catch(IOException e){
	    System.out.println("IO exception.");
	    System.out.println(e);
	}
	catch(IllegalStateException e){
	    System.out.println("Wrong URL!!");
	    System.out.println(e);
	}
	catch(NumberFormatException e){
	    System.out.println(e);
	}
    }
}

class Browse{
    //instance variables
    public String currentUrl,urlPath,contents,hostNameAndPort;
    public String[] listOfLinks = new String[256]; 
    public int linkNumber;
    //constructor
    public Browse(){
	currentUrl = "";
	urlPath = "";
	hostNameAndPort = "";
	listOfLinks[0] = "";
	contents = "";
	linkNumber = 0;//the number of links
    }
    //methods
    public void collectLinks(String contents){
	try{
	    String temp = "";
	    Pattern pat = Pattern.compile("<a href=\"(.*)\">");
	    Matcher mat = pat.matcher(contents);
	    if(mat.find()){
		temp = mat.group(1);
	    }
	    else{
		pat = Pattern.compile("<A HREF=\"(.*)\">");
		mat = pat.matcher(contents);
		if(mat.find()){
		    temp = mat.group(1);
		}
		else{
		    pat = Pattern.compile("<a(.*)href=\"(.*)\">");
		    mat = pat.matcher(contents);
		    if(mat.find()){
			temp = mat.group(2);
		    }
		}
	    }
	    //convert URL into absolute path
	    if(temp != ""){
		String st1 = "http://";
		String st2 = "/";
		
		if(temp.startsWith(st1)){
		    this.listOfLinks[this.linkNumber] = temp;
		}
		if(temp.startsWith(st2)){
		    this.listOfLinks[this.linkNumber] = "http://"+this.hostNameAndPort+temp;
		}
		if(!(temp.startsWith(st1))&&!(temp.startsWith(st2))){
		    this.listOfLinks[this.linkNumber] = currentUrl+temp;
		}
		this.linkNumber++;
	    }    
	}
	catch(NullPointerException e){
	    System.out.println("NullPointerException");
	    System.out.println(e);
	    System.exit(1);
	}
    }
}
