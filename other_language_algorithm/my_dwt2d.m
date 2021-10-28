%%%%%%%%%%%%%%%%%%%%%%
% Author: ZhaoY
% Platform: Windows10 MatlabR2019b
% Date: 10, 28, 2021 10:00
%%%%%%%%%%%%%%%%%%%%%%
clear all;
clc;
% f = imread("C:/Users/zqliu/source/repos/bmpv3/data/data2.bmp");
f = imread("C:/Users/zqliu/source/repos/bmpv3/data/lena512.bmp");
f_copy = f;
[ca,ch,cv,cd] = my_dwt2(f,'db1');
[ca1,ch1,cv1,cd1] = dwt2(f,'db1','mode','sym');
sum((ca-ca1)./ca1>1e-5,'all')
sum((ch-ch1)./ch1>1e-5,'all')
sum((cv-cv1)./cv1>1e-5,'all')
sum((cd-cd1)./cd1>1e-5,'all')
f_out = zeros(size(f));

for i = 1:10000
    [ca,ch,cv,cd] = dwt2(f,'haar');
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
figure(1)
subplot(1,2,1)
imshow(f_out,[])
f_out = zeros(size(f));
f = f_copy;

for i = 1:10000
    [ca,ch,cv,cd] = my_dwt2(f,'haar');
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
subplot(1,2,2)
imshow(f_out,[])
function [a,h,v,d] = my_dwt2(x,varargin)
%DWT2 Single-level discrete 2-D wavelet transform.
Lo_D = [sqrt(0.5),sqrt(0.5)];
Hi_D = [-sqrt(0.5),sqrt(0.5)];
% Check arguments for Extension and Shift.
shift = [0,0];
% Compute sizes.
lf = 2;
sx = size(x);
% Extend, Decompose &  Extract coefficients.
first = 2-shift;
last = sx+lf-1;
x = double(x);
if length(sx)==2    % 1-channel
    % y = wextend('addcol',dwtEXTM,x,sizeEXT);
    % y1 = [x(:,1),x,x(:,sx(2))];
    % sum(y~=y1,'all')
    y = [x(:,1),x,x(:,sx(2))];
    % a = [1,2,3;4,5,6]
    % a = [1,4,2,5,3,6]'
    % a' = [1,4,2,5,3,6] compress to row vector
%     z = conv2(y,Lo_D(:)','valid');
%     z1 = zeros(size(y,1),size(y,2)-1);
%     for ii = 1:size(z1,2)
%         z1(:,ii) = y(:,ii:ii+1)*Lo_D(:);
%     end
%     sum((z-z1)>1e-11,'all')
    z = zeros(size(y,1),size(y,2)-1);
    for ii = 1:size(z,2)
        z(:,ii) = y(:,ii:ii+1)*Lo_D(:);
    end
    
    a = convdown(z,Lo_D,first,last);
    h = convdown(z,Hi_D,first,last);
    % z = conv2(y,Hi_D(:)','valid');
    % same as above
    z = zeros(size(y,1),size(y,2)-1);
    for ii = 1:size(z,2)
        z(:,ii) = y(:,ii:ii+1)*Hi_D(:);
    end
    
    v = convdown(z,Lo_D,first,last);
    d = convdown(z,Hi_D,first,last);
% else                % 3-channels
%     a = cell(0,3);
%     h = cell(0,3);
%     v = cell(0,3);
%     d = cell(0,3);
%     for k = 1:3
%         y = wextend('addcol',dwtEXTM,x(:,:,k),sizeEXT);
%         z = conv2(y,Lo_D(:)','valid');
%         a{k} = convdown(z,Lo_D,dwtEXTM,sizeEXT,first,last);
%         h{k} = convdown(z,Hi_D,dwtEXTM,sizeEXT,first,last);
%         z = conv2(y,Hi_D(:)','valid');
%         v{k} = convdown(z,Lo_D,dwtEXTM,sizeEXT,first,last);
%         d{k} = convdown(z,Hi_D,dwtEXTM,sizeEXT,first,last);
%     end
%     a = cat(3,a{:});
%     h = cat(3,h{:});
%     v = cat(3,v{:});
%     d = cat(3,d{:});
end
end
%-------------------------------------------------------%
% Internal Function(s)
%-------------------------------------------------------%
function y = convdown(x,F,first,last)
% y = convdown(x,F,dwtEXTM,lenEXT,first,last)
y = x(:,first(2):2:last(2));
y = [y(1,:);y;y(size(y,1),:)];
% y = wextend('addrow',dwtEXTM,y,lenEXT);
% [row,col] = size(y);
% y1 = y';
% z = zeros(col,row-1);
% for ii = 1:size(z,2)
%   z(:,ii) = y1(:,ii:ii+1)*F(:);
% end
% y = conv2(y',F(:)','valid');
% sum((y-z)>1e-10,'all')
[row,col] = size(y);
y = y';
z = zeros(col,row-1);
for ii = 1:size(z,2)
  z(:,ii) = y(:,ii:ii+1)*F(:);
end
y = z';
y = y(first(1):2:last(1),:);
%-------------------------------------------------------%
end