%%%%%%%%%%%%%%%%%%%%%%
% Author: ZhaoY
% Platform: Windows10 MatlabR2019b
% Date: 09, 21, 2021 22:00
%%%%%%%%%%%%%%%%%%%%%%
clear all;
clc;
f = imread("C:/Users/zqliu/Desktop/lena512.bmp");
center_loc = size(f);
%figure(1);imshow(f);
f2=fft2(f);
f3=(log(abs(fftshift(f2))));
subplot(3,1,1);
imshow(f3,[]);
title("matlab 2d fft(fft2)");
% 1d fft -> 2d fft
tmp=zeros(center_loc(1),center_loc(2));
%-the first 1D-FFT, for each row
for i= 1:center_loc(1)
    tmp(i, :) = fft(f(i, :), center_loc(2));
end
%-transpose the tmp, for column-based 1d-fft
%-the second 1d-fft, for each column
for i= 1:center_loc(2)
    tmp(:, i) = fft(tmp(:, i), center_loc(1));
end
% different between 1d fft and 2d fft
disp(['fft2 vs 1dfft->2dfft: ',num2str(sum(sum(abs((tmp-f2))>1e-5)))]);

subplot(3,1,2);
f4 = log(abs(fftshift(tmp)));
imshow(f4,[]);
title("matlab 1d fft->2dfft");
% if length is even
% 0~(center_loc(1))/2-1 swap (center_loc(1))/2~center_loc(1)-1 for c
% 1~(center_loc(1))/2 swap (center_loc(1))/2+1~(center_loc(1))for matlab
% if length is odd(c idx)
% center_loc(1)/2 -> 0
% 0~center_loc(1)/2-1 ->center_loc(1)/2+1~center_loc(1)-1
% center_loc(1)/2+1~center_loc(1)-1 -> 0~center_loc(1)/2-1
tmp1(1:(center_loc(1))/2,:)=tmp(center_loc(1)/2+1:center_loc(1),:);
tmp1(center_loc(1)/2+1:center_loc(1),:)=tmp(1:(center_loc(1))/2,:);
tmp2(:,1:(center_loc(1))/2)=tmp1(:,center_loc(1)/2+1:center_loc(1));
tmp2(:,center_loc(1)/2+1:center_loc(1))=tmp1(:,1:(center_loc(1))/2);


disp(['fftshift vs my_shift: ',num2str(sum(sum(abs((tmp2-fftshift(f2)))>1e-5)))]);


subplot(3,1,3);
f5 = log(abs(tmp2));
imshow(f5,[]);
title("my\_2dfft and my\_fftshift");


