#include "DiagnosticProbe.h"

#include <iomanip>
#include <string>
#include <iomanip>

#include "PicParams.h"
#include "DiagParams.h"
#include "SmileiMPI.h"
#include "ElectroMagn.h"
#include "Field1D.h"
#include "Field.h"

using namespace std;

DiagnosticProbe::~DiagnosticProbe() {
}

void DiagnosticProbe::close() {
    if (fileId>0) {
        H5Fclose(fileId);
    }
}

void DiagnosticProbe::open(string file_name) {
    hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(plist_id, MPI_COMM_WORLD, MPI_INFO_NULL);
    fileId = H5Fcreate( file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
    H5Pclose(plist_id);
    
    string ver(__VERSION);
    
    // write version
    hid_t aid3  = H5Screate(H5S_SCALAR);
    hid_t atype = H5Tcopy(H5T_C_S1);
    H5Tset_size(atype, ver.size());
    H5Tset_strpad(atype,H5T_STR_NULLTERM);
    hid_t attr3 = H5Acreate2(fileId, "Version", atype, aid3, H5P_DEFAULT, H5P_DEFAULT);
    
    H5Awrite(attr3, atype, ver.c_str());
    
    H5Aclose(attr3);
    H5Sclose(aid3);
    H5Tclose(atype);    
}

string DiagnosticProbe::probeName(int p) {
    ostringstream prob_name("");
    prob_name << "p"<< setfill('0') << setw(4) << p;
    return prob_name.str();
}

void DiagnosticProbe::run(unsigned int np, ElectroMagn* EMfields, Interpolator* interp) {
    
    hsize_t dims = probeSize;
    hid_t  partMemSpace = H5Screate_simple(1, &dims, NULL);
    hsize_t nulldims = 0;
    hid_t  partMemSpaceNull = H5Screate_simple(1, &nulldims, NULL);
    
    vector<double> data(probeSize);
    
    unsigned int nprob=probeParticles[np].size();
    
    hid_t dataset_id = H5Dopen2(fileId, probeName(np).c_str(), H5P_DEFAULT);
    
    // All rank open all probes dataset
    hid_t file_space = H5Dget_space(dataset_id);
    
    // Get dataset existing dims
    hsize_t dimsO[3];
    H5Sget_simple_extent_dims(file_space, dimsO, NULL);
    
    // Increment dataset size
    hsize_t newDims[3] = { dimsO[0]+1, dimsO[1], dimsO[2]};
    H5Dset_extent(dataset_id, newDims);
    
    file_space = H5Dget_space(dataset_id);
    
    for (int count=0; count <nprob; count++) {
        if (probeId[np][count]==smpi_->getRank())
            (*interp)(EMfields,probeParticles[np],count,&Eloc_fields,&Bloc_fields);
        
        hsize_t count2[3];
        if  (probeId[np][count]==smpi_->getRank()) {
            count2[0] = 1;
            count2[1] = probeSize;
            count2[2] = 1;
        } else {
            count2[0] = 0;
            count2[1] = 0;
            count2[2] = 0;
        }
        hsize_t start[3] = { dimsO[0], 0, count};
        H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start, NULL, count2, NULL);
        
        //! here we fill the probe data!!!
        data[0]=Eloc_fields.x;
        data[1]=Eloc_fields.y;
        data[2]=Eloc_fields.z;
        data[3]=Bloc_fields.x;
        data[4]=Bloc_fields.y;
        data[5]=Bloc_fields.z;
        
        hid_t write_plist = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(write_plist, H5FD_MPIO_INDEPENDENT);
        
        if  (probeId[np][count]==smpi_->getRank()) {
            H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, partMemSpace, file_space, write_plist,&data[0]);
        } else {
            H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, partMemSpaceNull, file_space, write_plist,&data[0]);
        }
        
        H5Pclose( write_plist );
        
    }
    H5Sclose(file_space);
    
    H5Dclose(dataset_id);
    
    H5Fflush(fileId, H5F_SCOPE_GLOBAL );
    
    H5Sclose(partMemSpaceNull);
    H5Sclose(partMemSpace);
    
}
