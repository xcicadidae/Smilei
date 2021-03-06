#ifndef PROJECTOR_H
#define PROJECTOR_H

#include <complex>

#include "Params.h"
#include "Field.h"

class PicParams;
class Patch;

class ElectroMagn;
class Field;
class Particles;


//----------------------------------------------------------------------------------------------------------------------
//! class Projector: contains the virtual operators used during the current projection
//----------------------------------------------------------------------------------------------------------------------
class Projector {

public:
    //! Creator for the Projector
    Projector(Params&, Patch*);
    virtual ~Projector() {};
    virtual void mv_win(unsigned int shift) = 0;
    virtual void setMvWinLimits(unsigned int shift) = 0;

    //! Project global current charge (EMfields->rho_ , J), for initialization and diags
    virtual void operator() (double* rhoj, Particles &particles, unsigned int ipart, unsigned int type, std::vector<unsigned int> &b_dim) = 0;

    virtual void operator() (std::complex<double>* rhoj, Particles &particles, unsigned int ipart, unsigned int type, std::vector<unsigned int> &b_dim, int imode) {};

    //! Project global current densities if Ionization in Species::dynamics,
    virtual void operator() (Field* Jx, Field* Jy, Field* Jz, Particles &particles, int ipart, LocalFields Jion) = 0;

   //!Wrapper
    virtual void operator() (ElectroMagn* EMfields, Particles &particles, SmileiMPI* smpi, int istart, int iend, int ithread, int ibin, int clrw, bool diag_flag, bool is_spectral, std::vector<unsigned int> &b_dim, int ispec, int ipart_ref = 0) = 0;


    virtual void project_susceptibility(ElectroMagn* EMfields, Particles &particles, double species_mass, SmileiMPI* smpi, int istart, int iend,  int ithread, int ibin, std::vector<unsigned int> &b_dim, int ipart_ref = 0) {
        ERROR( "Envelope not implemented with this geometry and this order" );
    };

private:

};

#endif

