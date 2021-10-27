clear 
clc
a = double(0:7)';
dct1d(a)'

function b=dct1d(a)
%DCT Discrete cosine transform.
%
%   Y = DCT(X) returns the discrete cosine transform of X. The
%   vector Y is the same size as X and contains the discrete
%   cosine transform coefficients.  
%
%   Y = DCT(X,N) pads or truncates the vector X to length N
%   before transforming.  
%
%   If X is a matrix, the DCT operation is applied to each
%   column.  This transform can be inverted using IDCT.  
%
%   See also FFT,IFFT, and IDCT.
n = size(a,1);
m = size(a,2);
aa =a ; 
if ((rem(n,2)==1) || (~isreal(a))) % odd case
  % Form intermediate even-symmetric matrix.
  y = zeros(2*n,m);
  y(1:n,:) = aa;
  y(n+1:n+n,:) = flipud(aa);
  % Perform FFT
  yy = fft(y);
  % Compute DCT coefficients
  ww = (exp(-1i*(0:n-1)*pi/(2*n))/sqrt(2*n)).';
  ww(1) = ww(1) / sqrt(2);
  
  b = ww(:,ones(1,m)).*yy(1:n,:);
%   ww(:,ones(1,m))
%   yy(1:n,:)
else % even case
  % Re-order the elements of the columns of x
  y = [ aa(1:2:n,:); aa(n:-2:2,:) ];
  % Compute weights to multiply DFT coefficients
  ww = 2*exp(-1i*(0:n-1)'*pi/(2*n))/sqrt(2*n);
  ww(1) = ww(1) / sqrt(2);
  W = ww(:,ones(1,m));
  b = W .* fft(y);
end
if isreal(a) 
    b = real(b); 
end
end