#include <boost/program_options.hpp> 
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cstdlib>
#include <iostream> 
#include <string> 

#include "timemeasure.h"
#include "ABP.hpp"

namespace 
{ 
    const size_t ERROR_IN_COMMAND_LINE = 1; 
    const size_t SUCCESS = 0; 
    const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
    
} 

int main(int argc, char** argv) 
{ 
    CoulombABPOptions opt;
    try 
    { 
        /** 
         * Define and parse the program options 
         */ 
        
        namespace po = boost::program_options; 
        namespace fs = boost::filesystem; 

        po::options_description desc("Options"); 
        desc.add_options() 
        ("help", "Prints help messages.")
        ("totaltime,t", po::value<double>(&opt.tf), "Total simulation time.")
        ("particles,N", po::value<int>(&opt.N), "Number of particles.")
        ("prefix,o", po::value<std::string>(&opt.outputPrefix), "Path prefix for output files.")
        ("sigma1", po::value<double>(&opt.sigma1), "Major axis length of the ellipses.")
        ("sigma2", po::value<double>(&opt.sigma2), "Minor axis length of the ellipses.")
        ("D", po::value<double>(&opt.D), "Translational diffusion coefficient.")
        ("Dr", po::value<double>(&opt.Dr), "Rotational diffusion coefficient.")
        ("timestep", po::value<double>(&opt.dt), "Time step for the Euler-Maruyama scheme.")
        ("f", po::value<double>(&opt.f), "Amplitude of the constant drag force.")
        ("f0", po::value<double>(&opt.f0), "Bristlebot activity level.")
        ("mu", po::value<double>(&opt.mu), "Bristlebot mobility coefficient.")
        ("M", po::value<double>(&opt.M), "Mass of the tracer (in Bristlebot units).")
        ("delta", po::value<double>(&opt.delta), "Coulomb friction coefficient.")
        ("omega", po::value<double>(&opt.omega), "Chirality factor.")
        ("R", po::value<double>(&opt.R), "Radius of the arena.")
        ("R0", po::value<double>(&opt.R0), "Radius of the tracer.")
        ("kappa", po::value<double>(&opt.kappa), "Self-alignment parameter.")
        ("epsWall", po::value<double>(&opt.epsWall), "Lennard-Jones potential parameter for walls.")
        ("epsPart", po::value<double>(&opt.epsPart), "Lennard-Jones potential parameter for particles.")
        ("tfTrajectory", po::value<double>(&opt.tfTrajectory), "Trajectory output frequency.")
        ("eqtime", po::value<double>(&opt.teq), "Equilibration time before sampling starts.")
        ;
        
        po::variables_map vm; 
        try 
        { 
            po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 
            
            fs::ifstream in("setup.cfg");
            po::store(po::parse_config_file(in, desc, true), vm);
            
            /** --help option 
             */ 
            
            if ( vm.count("help")  ) 
            { 
                std::cout << "Basic Command Line Parameter App" << std::endl 
                << desc << std::endl; 
                return SUCCESS; 
            } 
            
            po::notify(vm); // throws on error, so do after help in case there are any problems 
        } 
        catch(po::error& e) 
        { 
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
            std::cerr << desc << std::endl; 
            return ERROR_IN_COMMAND_LINE; 
        } 
        
        std::cout << "Simulation Parameters:" << std::endl;
        std::cout << "Number of particles: " << opt.N << std::endl;
        std::cout << "Output path prefix: " << opt.outputPrefix << std::endl;
        std::cout << "Major axis length of ellipses: " << opt.sigma1 << std::endl;
        std::cout << "Minor axis length of ellipses: " << opt.sigma2 << std::endl;
        std::cout << "Time step (dt): " << opt.dt << std::endl;
        std::cout << "Translational diffusion coefficient (D): " << opt.D << std::endl;
        std::cout << "Rotational diffusion coefficient (Dr): " << opt.Dr << std::endl;
        std::cout << "Constant drag force amplitude (f): " << opt.f << std::endl;
        std::cout << "Bristlebot activity level (f0): " << opt.f0 << std::endl;
        std::cout << "Bristlebot mobility (mu): " << opt.mu << std::endl;
        std::cout << "Tracer mass (in Bristlebot units, M): " << opt.M << std::endl;
        std::cout << "Coulomb friction coefficient (delta): " << opt.delta << std::endl;
        std::cout << "Chirality factor (omega): " << opt.omega << std::endl;
        std::cout << "Self-alignment parameter (kappa): " << opt.kappa << std::endl;
        std::cout << "Lennard-Jones wall parameter (epsWall): " << opt.epsWall << std::endl;
        std::cout << "Lennard-Jones particle parameter (epsPart): " << opt.epsPart << std::endl;
        std::cout << "Arena radius (R): " << opt.R << std::endl;
        std::cout << "Tracer radius (R0): " << opt.R0 << std::endl;
        std::cout << "Equilibration time (teq): " << opt.teq << std::endl;
        std::cout << "Total simulation time (tf): " << opt.tf << std::endl;
        std::cout << "Trajectory output frequency (tfTrajectory): " << opt.tfTrajectory << std::endl;   

        CoulombABP sys(opt);

        sys.init();
        sys.equilibrate(opt.teq);
        sys.openOutputFiles();
        auto start = std::chrono::high_resolution_clock::now();
        sys.run(opt.tf);
        auto finish = std::chrono::high_resolution_clock::now();
        sys.closeOutputFiles();

        std::chrono::duration<double> elapsed = finish - start;

        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
        
    } 
    catch(std::exception& e) 
    { 
        std::cerr << "Unhandled Exception reached the top of main: " 
        << e.what() << ", application will now exit" << std::endl; 
        return ERROR_UNHANDLED_EXCEPTION; 
        
    } 
    
    return EXIT_SUCCESS;
    
}
