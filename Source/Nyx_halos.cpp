#include <iomanip>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <unistd.h>
#include <math.h>

using std::cout;
using std::cerr;
using std::endl;
using std::istream;
using std::ostream;
using std::pair;
using std::string;

#include <AMReX_CONSTANTS.H>
#include <Nyx.H>
#include <Nyx_F.H>
#include <Derive_F.H>

#include "AGNParticleContainer.H"

#if BL_USE_MPI
#include "MemInfo.H"
#endif

#include <reeber.H>

const int NyxHaloFinderSignal = 42;

// For debugging.
const int nghost0 = 0;
const int nghost1 = 1;
const int ncomp1 = 1;
const int comp0 = 0;

using namespace amrex;

void
Nyx::halo_find (Real dt)
{
   BL_PROFILE("Nyx::halo_find()");

   const Real * dx = geom.CellSize();

   amrex::MultiFab& new_state = get_new_data(State_Type);
   const BoxArray& simBA = new_state.boxArray();
   const DistributionMapping& simDM = new_state.DistributionMap();
   int simComp = new_state.nComp();

   // First copy the existing state into orig_state.
   MultiFab orig_state(simBA, simDM, simComp, nghost1);
   MultiFab::Copy(orig_state, new_state,
                  comp0, comp0, simComp, nghost1);

   // These are passed into the AGN particles' Redistribute
   int lev_min = 0;
   int lev_max = 0;
   int ngrow   = 0;

   /////////////////////////

   bool created_file = false;
   bool do_analysis = true;

   if (do_analysis || (reeber_int > 0 && nStep() % reeber_int == 0))
   {


       //       amrex::MultiFab reeberMF(simBA, simDM, 8, 0);
     //       amrex::MultiFab
     Vector<std::unique_ptr<MultiFab> > particle_mf;
     //     std::unique_ptr<MultiFab> particle_mf(new MultiFab(simBA, simDM, 4, 0));
       int cnt = 1;

       Real cur_time = state[State_Type].curTime();

       std::vector<Halo> reeber_halos;

       // global values updated when compute_average_density() called:
       /*
       average_gas_density;
       average_dm_density;
       average_neutr_density;
       average_total_density;
       */

       amrex::Vector<amrex::MultiFab*> state_levels;
       // Derive quantities and store in components 1... of MultiFAB

       amrex::Vector<amrex::IntVect> level_refinements;

       state_levels.resize(parent->finestLevel()+1);
       //Write all levels to Vector
       for (int lev = 0; lev <= parent->finestLevel(); lev++)
           {
             state_levels[lev]=&((get_level(lev)).get_new_data(State_Type));
         level_refinements.push_back(get_level(lev).fineRatio());
           }

       //This will fill all levels in particle_mf with lev_min=0
       Nyx::theDMPC()->AssignDensity(particle_mf, 0, 4, -1, 0);

       runReeberAnalysis(state_levels, particle_mf, Geom(), level_refinements, nStep(), do_analysis, reeber_halos);
       //////////////////////////////////////////////

       amrex::Real    halo_mass;
       amrex::IntVect halo_pos ;

       std::ofstream os;

       for (const Halo& h : reeber_halos)
       {
#if 0
           // We aren't actually writing to this file so don't create it
           if (reeber_halos_pos.size() > 0)
           {
              if (!created_file)
                 os.open(amrex::Concatenate(amrex::Concatenate("debug-halos-", nStep(), 5), ParallelDescriptor::MyProc(), 2));
              created_file = true;
           }
#endif
           halo_mass = h.totalMass;
           halo_pos  = h.position;

           if (halo_mass > mass_halo_min)
           {
                amrex::Real x = (halo_pos[0]+0.5) * dx[0];
                amrex::Real y = (halo_pos[1]+0.5) * dx[1];
                amrex::Real z = (halo_pos[2]+0.5) * dx[2];
   
                amrex::Real mass = mass_seed;

                int lev = 0;
                int grid = 0;
                int tile = 0;

                // Note that we are going to add the particle into grid 0 and tile 0 at level 0 -- 
                //      this is not actually where the particle belongs, but we will let the Redistribute call
                //      put it in the right place

                //                Nyx::theAPC()->AddOneParticle(lev,grid,tile,mass,x,y,z); // ,u,v,w);
                std::cout << "ADDED A PARTICLE AT " << x << " " << y << " " << z << " WITH MASS " << mass << std::endl;
           }
       } // end of loop over creating new particles from halos


     }
}
