import ROOT
from math import sqrt

ndim = 2
covariance = ROOT.TMatrixDSym(ndim)
errors = [ 0.7, 1 ]
correlations = [ [1, 0.5], [0.5, 1] ]
transform = [ [1, 0], [0, 1] ]

for ix in xrange(ndim):
    for iy in xrange(ndim):
        covariance[ix][iy] = correlations[ix][iy]*errors[ix]*errors[iy]
covariance.Print()

eigen = ROOT.TMatrixDSymEigen(covariance)
vectors = eigen.GetEigenVectors()
values  = eigen.GetEigenValues()

vectors.Print()
values.Print()


for ix in xrange(ndim):
    for iy in xrange(ndim):
        transform[ix][iy] = vectors(ix,iy) * sqrt(values(iy))

print transform
