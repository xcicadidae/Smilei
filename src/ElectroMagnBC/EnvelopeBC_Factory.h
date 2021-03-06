
#ifndef ENVELOPEBC_FACTORY_H
#define ENVELOPEBC_FACTORY_H

#include "EnvelopeBC.h"
#include "EnvelopeBC3D_refl.h"

#include "Params.h"

//  --------------------------------------------------------------------------------------------------------------------
//! Constructor for the Envelope Boundary conditions factory
//  --------------------------------------------------------------------------------------------------------------------
class EnvelopeBC_Factory {
    
public:
    
    static std::vector<EnvelopeBC*> create(Params& params, Patch* patch) {
        
        std::vector<EnvelopeBC*> EnvBoundCond;

        // periodic (=NULL) boundary conditions
        EnvBoundCond.resize(2*params.nDim_field, NULL);
        
        // -----------------
        // For 3Dcartesian Geometry only for the moment
        // -----------------
        if ( params.geometry == "3Dcartesian" ) {
            
            for (unsigned int ii=0;ii<2;ii++) {
                // X DIRECTION
                // reflective bcs
                if ( params.Env_BCs[0][ii] == "reflective" ) {
                    EnvBoundCond[ii] = new EnvelopeBC3D_refl(params, patch, ii);
                }
                // else: error
                else {
                    ERROR( "Unknown Envelope x-boundary condition `" << params.Env_BCs[0][ii] << "`");
                }
                
                // Y DIRECTION
                // reflective bcs
                if ( params.Env_BCs[1][ii] == "reflective" ) {
                    EnvBoundCond[ii+2] = new EnvelopeBC3D_refl(params, patch, ii+2);
                }
                // else: error
                else {
                    ERROR( "Unknown Envelope y-boundary condition `" << params.Env_BCs[1][ii] << "`");
                }

                // Z DIRECTION
                // reflective bcs 
                if ( params.Env_BCs[2][ii] == "reflective" ) {
                    EnvBoundCond[ii+4] = new EnvelopeBC3D_refl(params, patch, ii+4);
                }
                // else: error
                else  {
                    ERROR( "Unknown Envelope z-boundary condition `" << params.Env_BCs[2][ii] << "`");
                }
            }
            
        }//3Dcartesian       


        // OTHER GEOMETRIES ARE NOT DEFINED ---
        else {
            ERROR( "Unknown geometry : " << params.geometry );
        }
        
        return EnvBoundCond;
    }
    
};

#endif

