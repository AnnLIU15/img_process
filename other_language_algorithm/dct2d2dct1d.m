%%%%%%%%%%%%%%%%%%%%%%
% Author: ZhaoY
% Platform: Windows10 MatlabR2019b
% Date: 10, 27, 2021 19:00
%%%%%%%%%%%%%%%%%%%%%%
clear
clc;
% a = imread("C:/Users/zqliu/source/repos/bmpv3/data/lena512.bmp");
% a = double(a);
a =zeros(1024,512);
a(1:600,1:400) = double(imread("C:/Users/zqliu/source/repos/bmpv3/data/data2.bmp"));
% a=dct2(a);
% a =fftshift(a);
% a_tmp = log(abs(a));
% imshow(a_tmp,[])

origin_b = dct2(a);
dev_b = dct2d(a);
beta_b =dct2d_1d(a);
f1 = log(abs(origin_b));
subplot(3,1,1);

imshow(f1,[]);
f2 = log(abs(dev_b));
subplot(3,1,2);

imshow(f2,[]);
f3 = log(abs(beta_b));
fmin = min(min(f3));
fmax = max(max(f3));
f3 = 255.0*(f3-fmin)/(fmax-fmin);
f3 = uint8(f3);
subplot(3,1,3);
imshow(f3);
function b=dct2d(arg1)
% from dct2 cut some unuse information
b = dct(dct(arg1).').';
end

function c=dct2d_1d(arg1)
[m,n]=size(arg1);
b =zeros(m,n);
c =zeros(m,n);
for ii=1:m
    b(ii,:) = dct(arg1(ii,:));
end
for ii=1:n
    c(:,ii) = dct(b(:,ii));
end
end
