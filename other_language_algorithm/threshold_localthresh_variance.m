clear all;
clc;
f = imread("C:/Users/zqliu/source/repos/bmp_process/data/yeast-cells.bmp");
g = localthresh(f,ones(3),30,2);
imshow(g,[])

function g=localthresh(f,nhood,a,b)
%??????
%nhood??????
%???g=localthresh(f,ones(3),30,1.5,'global')
%?????????
f=double(f);
f2 = zeros(size(f)+2);
f2(2:size(f,1)+1,2:size(f,2)+1)=f;
SIG2 = zeros(size(f));
for ii = 1: size(f,1)
    parfor jj = 1:size(f,2)
        SIG2(ii,jj) = std2(f2(ii:ii+2,jj:jj+2));
    end
end
MEAN=mean2(f);

g=(f>a*SIG2)&(f>b*MEAN);
end


function J = stdfilt(I,h)
%STDFILT Local standard deviation of image.
J = images.internal.algstdfilt(I, h);
end
