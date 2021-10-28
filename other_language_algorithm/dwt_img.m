%%%%%%%%%%%%%%%%%%%%%%
% Author: ZhaoY
% Platform: Windows10 MatlabR2019b
% Date: 10, 28, 2021 10:00
%%%%%%%%%%%%%%%%%%%%%%
clear all;
clc;
f = imread("C:/Users/zqliu/source/repos/bmpv3/data/lena512.bmp");
% f = zeros(1024,512);
% f(1:600,1:400) = double( imread("C:/Users/zqliu/source/repos/bmpv3/data/data2.bmp"));
f_out = zeros(size(f));

for i = 1:10000
    [ca,ch,cv,cd] = dwt2(f,'db1','mode','sym');
    max_ca = max(ca,[],'all'); min_ca = min(ca,[],'all');
    f = uint8(round((ca - min_ca)/(max_ca-min_ca)*255));
    idx = size(f);
    f_out(1:idx(1),1:idx(2)) = ca;
    f_out(1:idx(1),idx(2)+1:idx(2)*2) = ch;
    f_out(idx(1)+1:2*idx(1),1:idx(2)) = cv;
    f_out(idx(1)+1:2*idx(1),idx(2)+1:idx(2)*2) = cd;
    if idx(1)<2 || idx(2)<2
        break;
    end
end
figure(2)
imshow(f_out,[])