#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
//#include <boost/uuid/uuid.hpp>            // uuid class
//#include <boost/uuid/uuid_generators.hpp> // generators
//#include <boost/uuid/uuid_io.hpp> // streaming operators

using namespace std;

struct staging 
{
    bool StageIn; // default is stage in, if set to false then it's stage out
    string SourceFile;
    string LocalSourcePath;
    string TargetFile;
    string LocalTargetPath;
    string CreationFlag;
    bool DeleteOnTermination;
    bool Unpack;
    string Scratch;

};

string clockTimeConversion(string t)
{
    int hours = stoi(t.substr(0,t.find_first_of(":")));
    t = t.substr(t.find_first_of(":")+1);
    int minutes = stoi(t.substr(0,t.find_first_of(":")));
    t = t.substr(t.find_first_of(":")+1);
    int seconds = stoi(t);
    int total_seconds = seconds + (minutes*60) + (hours*60*60);
    string return_string = to_string(total_seconds);
    return return_string;
    
}

string memoryUnitConversion(string m)
{   
    long converted_num = stoi(m.substr(0,m.length()-2));
    if (m.find_first_of("g") != -1)
    {
        converted_num = converted_num*1024*1024*1024;
        m = to_string(converted_num);
        return m;
    }
    else if (m.find_first_of("m") != -1)
    {
        converted_num = converted_num*1024*1024;
        m = to_string(converted_num);
        return m;
    }
    else if (m.find_first_of("k") != -1)
    {
        converted_num = converted_num*1024;
        m = to_string(converted_num);
        return m;
    }
    else if (m.find_first_of("b") != -1)
    {
        return m.substr(0,m.length()-1);
    }
    else 
    {
        cout << "You must include a unit for memory requirement." << endl;
        exit(1);
    }
    
    
}

void processDirectivesIntoJSDLFile(map<string,string> directives, vector<staging> stagingCommands, string GUID, string rns_prefix, string jsdl_outfile)
{

    ofstream jsdl(jsdl_outfile); // stream write to jsdl file
    map <string, string> :: iterator itr;

    string JobDescriptionStart =  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?> \n<jsdl:JobDefinition xmlns:jsdl-hpcpa=\"http://schemas.ggf.org/jsdl/2006/07/jsdl-hpcpa\" xmlns:hpcfse-ac=\"http://schemas.ogf.org/hpcp/2007/11/ac\" xmlns:jsdl-sweep=\"http://schemas.ogf.org/jsdl/2009/03/sweep\" xmlns:jsdl-posix=\"http://schemas.ggf.org/jsdl/2005/11/jsdl-posix\" xmlns:jsdl-spmd=\"http://schemas.ogf.org/jsdl/2007/02/jsdl-spmd\" xmlns:jsdl=\"http://schemas.ggf.org/jsdl/2005/11/jsdl\" xmlns:wss=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\" xmlns:ns8=\"http://vcgr.cs.virginia.edu/jsdl/genii\" xmlns:ns9=\"http://schemas.ogf.org/jsdl/2009/03/sweep/functions\">\n<jsdl:JobDescription>";
    string JobDescriptionEnd = "\n</jsdl:JobDescription>\n</jsdl:JobDefinition>";
    string Application = "\n<jsdl:Application>";
    string POSIXApplication = "\n<jsdl-posix:POSIXApplication>";
    string Resources = "\n<jsdl:Resources>\n<jsdl:OperatingSystem>\n<jsdl:OperatingSystemType>\n<jsdl:OperatingSystemName>Linux</jsdl:OperatingSystemName>\n</jsdl:OperatingSystemType>\n</jsdl:OperatingSystem>\n<jsdl:CPUArchitecture>\n<jsdl:CPUArchitectureName>x86</jsdl:CPUArchitectureName>\n</jsdl:CPUArchitecture>";
    string JobIdentification =  "\n<jsdl:JobIdentification>";
    string DataStaging = "";
    

    for (itr = directives.begin(); itr != directives.end(); ++itr)
    {
        
        if (itr-> first == "error")
        {
            POSIXApplication += "\n<jsdl-posix:Error>";
            POSIXApplication += itr->second;
            POSIXApplication += "</jsdl-posix:Error>";
        }
	else if (itr-> first == "executable")
	  {
	    POSIXApplication += "\n<jsdl-posix:Executable>";
	    POSIXApplication += itr->second;
	    POSIXApplication +="</jsdl-posix:Executable>";
	  }
        
        else if (itr-> first == "input")
        {
            POSIXApplication += "\n<jsdl-posix:Input>";
            POSIXApplication += itr->second;
            POSIXApplication += "</jsdl-posix:Input>";
        }

        else if (itr-> first == "output")
        {
            POSIXApplication += "\n<jsdl-posix:Output>";
            POSIXApplication += itr->second;
            POSIXApplication += "</jsdl-posix:Output>";
        }

        else if (itr -> first == "time")
        {
            string WallclockTime = "\n<ns8:WallclockTime>\n<jsdl:UpperBoundedRange exclusiveBound=\"false\">";
            WallclockTime += clockTimeConversion(itr->second);
            WallclockTime += "</jsdl:UpperBoundedRange>\n</ns8:WallclockTime>";
            Resources += WallclockTime;
        }

        else if (itr -> first == "job-name")
        {
            JobIdentification += "\n<jsdl:JobName>";
            JobIdentification += itr->second;
            JobIdentification += "</jsdl:JobName>";
        }

        else if (itr -> first == "mem")
        {
            Resources+= "\n<jsdl:TotalPhysicalMemory>\n<jsdl:UpperBoundedRange exclusiveBound=\"false\">";
            string num_bytes = memoryUnitConversion(itr->second); 
            Resources += num_bytes;     
            Resources+= "</jsdl:UpperBoundedRange>\n</jsdl:TotalPhysicalMemory>";
        }   
    }
    

    for (int i = 0; i < stagingCommands.size(); ++i)
    { 
        DataStaging += "\n<jsdl:DataStaging>\n<jsdl:FileName>";
        DataStaging += stagingCommands[i].SourceFile; 
        DataStaging += "</jsdl:FileName>\n";
        DataStaging += "<jsdl:CreationFlag>"; // What should be the default creation flag?
        DataStaging += stagingCommands[i].CreationFlag;
        DataStaging += "</jsdl:CreationFlag><jsdl:DeleteOnTermination>";

        if (stagingCommands[i].DeleteOnTermination){DataStaging+= "true</jsdl:DeleteOnTermination>\n<jsdl:HandleAsArchive>";}
        else {DataStaging += "false</jsdl:DeleteOnTermination>\n<jsdl:HandleAsArchive>";}
        
        if (stagingCommands[i].Unpack){DataStaging+= "true</jsdl:HandleAsArchive>";}
        else {DataStaging += "false</jsdl:HandleAsArchive>";}


        if (stagingCommands[i].StageIn)
        {
            DataStaging += "\n<jsdl:Source>\n<jsdl:URI>";
            DataStaging += rns_prefix;
            DataStaging += GUID + "/";
            DataStaging += stagingCommands[i].SourceFile; 
            // Check to see if source file is in working directory or is a path, if it's a path then pull just the file name (search for last '/')
            DataStaging += "</jsdl:URI>\n</jsdl:Source>";
        }
        else
        {
            DataStaging += "\n<jsdl:Target>\n<jsdl:URI>";
            DataStaging += rns_prefix;
            DataStaging += GUID + "/";
            DataStaging += stagingCommands[i].SourceFile;
            DataStaging +="</jsdl:URI>\n</jsdl:Target>";

        }

        DataStaging+="\n</jsdl:DataStaging>";
    }

    // Add the different parts generated above to the jsdl file we are creating
    jsdl << JobDescriptionStart;

    if (JobIdentification != "\n<jsdl:JobIdentification>"){
        JobIdentification += "\n</jsdl:JobIdentification>";
        jsdl << JobIdentification;
    }
    if (POSIXApplication != "\n<jsdl-posix:POSIXApplication>"){
        POSIXApplication +=  "\n</jsdl-posix:POSIXApplication>";
        Application += POSIXApplication;
    }
    if (Application != "\n<jsdl:Application>"){
        Application += "\n</jsdl:Application>";
        jsdl << Application;
    }

    Resources += "\n</jsdl:Resources>";
    jsdl << Resources;
   

    jsdl << DataStaging;
    jsdl << JobDescriptionEnd;
    


}

void createStageInAndOutFile(vector<staging> stagingCommands, string GUID, string rns_prefix, string filename, string sh_outfile)
{
    ofstream stagein(filename + ".stagein"); 
    ofstream stageout(filename + ".stageout");

    char buff[1024];
    char *path;
    path = getcwd(buff,1023);
    string cwd = path;

    stagein << "cp local:"<< cwd << "/" << sh_outfile << " " << rns_prefix << GUID << "/" << sh_outfile << endl;
    for (int i = 0; i < stagingCommands.size(); ++i)
    {
        if (stagingCommands[i].StageIn)
        {
	  stagein << "cp local:" << stagingCommands[i].LocalSourcePath << " " << rns_prefix << GUID << "/" << stagingCommands[i].SourceFile << endl;
        }
        else 
        {
	  stageout << "cp " << rns_prefix << GUID << "/" << stagingCommands[i].SourceFile << " local:" << stagingCommands[i].LocalTargetPath << endl;
        }
    }
}


// exit with non zero return value


int main(int argc, char* argv[])
{
    
    string rns_prefix = "rns:/etc/tmp/lhstesting/";
    /*
    boost::uuids::random_generator gen;
    boost::uuids::uuid id = gen();
    string GUID = to_string(id);
    */
    string GUID = "b93c542a-98a4-44f0-be54-d16503e983bd";
    string filename = argv[1];

    map<string,string> Directives;
    vector<staging> StagingCommands;

    ifstream file (filename); 
    if (!file) { 
        exit(1);
    }

    
    filename = filename.substr(0, filename.length() - 3);
    string sh_outfile = filename + "_.sh";


    ofstream shell(sh_outfile); // stream write to sh file
    string sent;
    Directives.insert(pair<string,string>("executable",sh_outfile));
    
    while (getline (file, sent)) // while there are lines left to read
        {
            if (sent.substr(0,7) == "#SBATCH") { // if line starts with #SBATCH
            
                string stripped = sent.substr(10,sent.length()); // strip #SBATCH and other unnecessary information 
                int equals_index = stripped.find('=');
                string directiveUsed = stripped.substr(0,equals_index);
                string optionUsed = stripped.substr(equals_index+1);
                Directives.insert(pair<string,string>(directiveUsed, optionUsed));
		
                
            }


            else if (sent.substr(0,4) == "#CCC"){ 
                staging StagingOptions; // Instantiate the struct that will hold the data staging options/information
                StagingOptions.SourceFile = "";
                StagingOptions.LocalSourcePath = "";
                StagingOptions.TargetFile = "";
                StagingOptions.LocalTargetPath = "";
                StagingOptions.CreationFlag = "overwrite";
                StagingOptions.DeleteOnTermination = false;
                StagingOptions.Unpack = false;
                StagingOptions.Scratch = "";
                
                string Stripped= sent.substr(5,sent.length());
                
                // If it's stage-in command
                if (Stripped.substr(0,8) == "STAGE-IN"){
                    StagingOptions.StageIn = true; 
                    Stripped = Stripped.substr(9,Stripped.length()); 
                }
                // Otherwise it must be a stage-out command
                else{
                    StagingOptions.StageIn= false;
                    Stripped = Stripped.substr(10,Stripped.length());
                }
                
                // Grab first file path and strip it from remaining command
                string source_file_path= Stripped.substr(0, Stripped.find_first_of(" \t"));

                if (StagingOptions.StageIn && source_file_path[0] != '/')
                {
		  char buff[1024];
		  char *path;
		  path = getcwd(buff,1023);
		  string cwd = path;
                  cwd += "/"+ source_file_path;
                  StagingOptions.LocalSourcePath = cwd;
                }
                string source_file = source_file_path;
                // Strip the source file path to just the file name
                while (source_file.find_first_of("/") != -1){
                   source_file = source_file.substr(source_file.find_first_of("/")+1);
                }
                StagingOptions.SourceFile = source_file; 
                Stripped = Stripped.substr(Stripped.find_first_of(" \t") + 1);


                // Grab second file path and strip it from remaining command
                string target_file_path = Stripped.substr(0, Stripped.find_first_of(" \t"));
                if (!(StagingOptions.StageIn) && target_file_path[0] != '/')
		  {
		    char buff[1024];
		    char *path;
		    path = getcwd(buff,1023);
		    
                    string cwd = path;
		    cwd += "/" + target_file_path;
                    StagingOptions.LocalTargetPath = cwd;
                }
                string target_file = target_file_path;
                while (target_file.find_first_of("/") != -1){ 
                   target_file = target_file.substr(target_file.find_first_of("/")+1);
                }
                StagingOptions.TargetFile= target_file;
                Stripped = Stripped.substr(Stripped.find_first_of(" \t") + 1);

                
                // Grab each word from remaining command and add it to the staging object based on what option it  should trigger
                while (Stripped != ""){
                    // Take the first optional parameter (if it exists) and add it to the staging object based on the keyword
                    int next_space =  Stripped.find_first_of(" \t");
                    string next_directive = Stripped.substr(0, next_space);

                    if (next_directive == "overwrite"){
                        StagingOptions.CreationFlag = "overwrite";
                    }
                    else if (next_directive == "append"){
                        StagingOptions.CreationFlag = "append";
                    }
                    else if (next_directive == "do-not-overwrite"){
                        StagingOptions.CreationFlag = "do-not-overwrite";
                    }
                    else if (next_directive == "delete-on-termination"){
                        StagingOptions.DeleteOnTermination = true;
                    }
                    else if (next_directive == "unpack"){
                        StagingOptions.Unpack = true;
                    }
                    else if (next_directive.length() >= 8 && next_directive.substr(0,8) == "scratch="){
                        StagingOptions.Scratch = next_directive.substr(8,next_directive.length());
                    }
                    if (next_space == -1){Stripped = "";}
                    else{
                        Stripped = Stripped.substr(next_space + 1);
                    }

                
                }
                // Add each staging command to a vector
                StagingCommands.push_back(StagingOptions);

            }
            else{
                shell << sent << endl;
            }
        }
    
    processDirectivesIntoJSDLFile(Directives, StagingCommands, GUID, rns_prefix, filename + ".jsdl");
    createStageInAndOutFile(StagingCommands, GUID, rns_prefix, filename, sh_outfile);
    cout << GUID << endl;
    
}

