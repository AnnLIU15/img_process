clear
close all;
clc
img = imread("C:/Users/zqliu/source/repos/bmpv3/data/data4.bmp");
tmp = im2double(img);
img2 = img(:,:,1);
tmp = tmp(:,:,1);
hsi_f = rgb2hsi(img);
hsi_f(:,:,3)=histeq(hsi_f(:,:,3));
f_equ = hsi2rgb(hsi_f);
imshow(f_equ)

function hsi=rgb2hsi(img)
img=im2double(img);
r=img(:,:,1);g=img(:,:,2);b=img(:,:,3);
num=0.5*(2*r-g-b);
den=sqrt((r-g).^2+(r-b).*(g-b));
theta=acos(num./(den+eps));
S= 1-3*min(min(r,g),b)./(r+g+b+eps);
H= theta;
H(b>g) = 2*pi-H(b>g);
H(S==0)=0;
hsi(:,:,1)=H;
hsi(:,:,2)=S;
hsi(:,:,3)=(r+g+b)/3;
end

function rgb=hsi2rgb(img)
rows = length(img(:,1));
cols = length(img(1,:))/length(img(1,1,:));
rgb=img;
for ii=1:rows
    for jj=1:cols
        h=img(ii,jj,1);
        s=img(ii,jj,2);
        i=img(ii,jj,3);
        if 0<=h<=2*pi/3
            b=i*(1-s);
            r=i*(1+s*cos(h)/cos(pi/3-h));
            g=3*i-(b+r);
        elseif 2*pi/3<=h<=4*pi/3
            h = h-2*pi/3;
            r=i*(1-s);
            g=i*(1+s*cos(h)/cos(pi/3-h));
            b=3*i-(r+g);
        else
            h = h-4*pi/3;
            g=i*(1-s);
            b=i*(1+s*cos(h)/cos(pi/3-h));
            r=3*i-(b+g);
        end
        rgb(ii,jj,:)=[r,g,b];
    end
end
end

