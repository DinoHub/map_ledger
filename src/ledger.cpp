#include "ledger.hpp"

#include <cstdlib>
#include <openssl/sha.h>
#include <iostream>

#include "base64.h"

template <typename T>
VoxelGrid<T>::VoxelGrid(
    int n_x,
    int n_y,
    int n_z,
    int res_x,
    int res_y,
    int res_z)
    : n_x(n_x)
    , n_y(n_y)
    , n_z(n_z)
    , res_x(res_x)
    , res_y(res_y)
    , res_z(res_z)
{
    int n_voxels = n_x*res_x * n_y*res_y * n_z*res_z;
    
    data = (T*) malloc(n_voxels * sizeof(T));
}

template <typename T>
VoxelGrid<T>::~VoxelGrid()
{
    free(data);
}

template <typename T>
Coordinate VoxelGrid<T>::ind2xyz(int ind)
{
    Coordinate c;

    std::div_t dv{};
    dv = std::div(ind, n_y*res_x*n_z*res_z);
    c[0] = dv.quot;
    dv = std::div(dv.rem, n_z*res_z);
    c[1] = dv.quot;
    c[2] = dv.rem;

    return c;
}

template <typename T>
int VoxelGrid<T>::xyz2ind(Coordinate c)
{
    return xyz2ind(c[0], c[1], c[2]);
}

template <typename T>
int VoxelGrid<T>::xyz2ind(int x, int y, int z)
{
    return x*(n_y*res_y*n_z*res_z) + y*n_z*res_z + z;
}

template <typename T>
void VoxelGrid<T>::sub_inds(int s_x, int s_y, int s_z, std::vector<int>& inds)
{
    for (int x = s_x*res_x; x < (s_x + 1)*res_x; x++)
    {
        for (int y = s_y*res_y; y < (s_y + 1)*res_y; y++)
        {
            for (int z = s_z*res_z; z < (s_z + 1)*res_z; z++)
            {
                inds.push_back(xyz2ind(x, y, z));
            }
        }
    }
}

template <typename T>
void VoxelGrid<T>::sub_hash(int s_x, int s_y, int s_z, std::array<unsigned char, 20>& md)
{
    // get subgrid indices
    std::vector<int> inds;
    inds.reserve(res_x * res_y * res_z);
    sub_inds(s_x, s_y, s_z, inds);

    // get data into contiguous buffer
    std::vector<T> s_data;
    s_data.reserve(res_x * res_y * res_z);
    
    for (int i : inds)
    {
        s_data.push_back(data[i]);
    }

    // compute SHA1 digest
    SHA1((unsigned char*) s_data.data(), res_x*res_y*res_z*sizeof(T), (unsigned char *) md.data());
}

template <typename T>
std::string VoxelGrid<T>::sub_hash_str(int s_x, int s_y, int s_z)
{
    std::array<unsigned char, 20> md;
    sub_hash(s_x, s_y, s_z, md);
    return base64_encode(md.data(), 20);
}

template <typename T>
void VoxelGrid<T>::sub_hashes_str(std::vector<std::string>& hashes)
{
    int i = 0;

    for (int s_x = 0; s_x < n_x; s_x++)
    {
        for (int s_y = 0; s_y < n_y; s_y++)
        {
            for (int s_z = 0; s_z < n_z; s_z++)
            {
                hashes[i] = sub_hash_str(s_x, s_y, s_z);
                i++;
            }
        }
    }
}

template <typename T>
T &VoxelGrid<T>::operator()(int x, int y, int z)
{
    int ind = xyz2ind(x, y, z);
    return data[ind];
}

template <typename T>
T &VoxelGrid<T>::operator()(int i)
{
    return data[i];
}

template <typename T>
void print_grid(VoxelGrid<T> & grid) {
    for (int y = 0; y < grid.n_y*grid.res_y; y++)
    {
        for (int x = 0; x < grid.n_x*grid.res_x; x++)
        {
            std::cout << " | " << grid(x, y, 0);
        }
        std::cout << " | " << std::endl;
    }
}


int main(int argc, char* argv[])
{
    int SUB_X = 5, SUB_Y = 4, SUB_Z = 1, RES_X = 5, RES_Y = 5, RES_Z = 1;

    VoxelGrid<int> grid(SUB_X, SUB_Y, SUB_Z, RES_X, RES_Y, RES_Z);

    std::vector<std::string> hashes(SUB_X*SUB_Y*SUB_Z);

    std::cout << "Before modification\n===========================" << std::endl;

    print_grid(grid);
    
    grid.sub_hashes_str(hashes);

    std::cout << "Hashes:" << std::endl;
    for (int i = 0; i < hashes.size(); i++)
    {
        std::cout << i << ":\t" << hashes[i] << std::endl;
    }

    // Modification
    // std::vector<int> sub_inds;
    // grid.sub_inds(2, 2, 0, sub_inds);
    
    // for (int i : sub_inds)
    // {
    //     grid(i) = 1;
    // }

    grid(17) = 5;

    std::cout << std::endl;

    std::cout << "After modification\n===========================" << std::endl;

    print_grid(grid);

    grid.sub_hashes_str(hashes);

    std::cout << "Hashes:" << std::endl;
    for (int i = 0; i < hashes.size(); i++)
    {
        std::cout << i << ":\t" << hashes[i] << std::endl;
    }

    return 0;
}