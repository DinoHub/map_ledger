#ifndef LEDGER_HPP_
#define LEDGER_HPP_

#include <array>
#include <vector>
#include <cstdint>
#include <string>

using Coordinate = std::array<int, 3>;

struct SubGrid {
    void serialize(std::vector<uint8_t>& buf);
};

template <typename T>
class VoxelGrid
{
public:
    VoxelGrid(
        int n_x, 
        int n_y,
        int n_z,
        int res_x,
        int res_y,
        int res_z
    );
    ~VoxelGrid();

    Coordinate ind2xyz(int ind);
    int xyz2ind(Coordinate c);
    int xyz2ind(int x, int y, int z);
    void sub_inds(int s_x, int s_y, int s_z, std::vector<int>& inds);
    void sub_hash(int s_x, int s_y, int s_z, std::array<unsigned char, 20>& md);
    std::string sub_hash_str(int s_x, int s_y, int s_z);
    void sub_hashes_str(std::vector<std::string>& hashes);

    T& operator() (int x, int y, int z);
    T& operator() (int i);

    int n_x;       // nuumber of subgrids in x
    int n_y;       // nuumber of subgrids in y
    int n_z;       // nuumber of subgrids in z
    int res_x;     // number of voxels in x for each subgrid
    int res_y;     // number of voxels in y for each subgrid
    int res_z;     // number of voxels in z for each subgrid

private:
    T* data;
};


#endif // LEDGER_HPP_