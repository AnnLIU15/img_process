%%%%%%%%%%%%%%%%%%%%%%
% Author: ZhaoY
% Platform: Windows10 MatlabR2019b
% Date: 10, 27, 2021 19:00
%%%%%%%%%%%%%%%%%%%%%%
clear 
clc
% this script is testing whether 1DDCT can be implemented using 1DFFT
a = double([0:100]*(1-1j))';
disp(['complex var error number:',num2str(sum(abs(dct1d(a) -dct(a))>1e-10,'ALL'))])
a = double(0:100)';
disp(['real error number:',num2str(sum(abs(dct1d(a) -dct(a))>1e-10,'ALL'))])
function b=dct1d(a)
%DCT Discrete cosine transform.
% the algrothm copy from `matlab dct` function, you can use help dct to
% find it 
n = size(a,1);
m = size(a,2);
aa =a ; 
if ((rem(n,2)==1) || (~isreal(a))) % odd case
    % Due to the padding, it must be even case
    % and the image hasn't iamg, so may be you
    % can ignore this branch
  y = zeros(2*n,m);
  y(1:n,:) = aa;
  y(n+1:n+n,:) = flipud(aa);
  % if y = 1 2 3 4
  % then after the operation above,
  % y will become 1 2 3 4 4 3 2 1
  yy = fft(y);
  % From the formula
  ww = (exp(-1i*(0:n-1)*pi/(2*n))/sqrt(2*n)).';
  ww(1) = ww(1) / sqrt(2);
  b = ww(:,ones(1,m)).*yy(1:n,:);
else % even case
    % if y =0 1 2 3 4 5 6 7
    % y = 0 2 4 6 7 5 3 1
    % 67 should figure out a way to map them to this distribution hh
  y = [ aa(1:2:n,:); aa(n:-2:2,:) ];
  % some different between odd/complex situation, pay attention to the 2
  ww = 2*exp(-1i*(0:n-1)'*pi/(2*n))/sqrt(2*n);
  ww(1) = ww(1) / sqrt(2);
  W = ww(:,ones(1,m));
  b = W .* fft(y);
end
% Discarding the imaginary part
if isreal(a) 
    b = real(b); 
end
end