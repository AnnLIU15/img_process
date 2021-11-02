clear
clc
x=reshape((0:262143)*(1-1j),[512,512]).';
sx = size(x);
shift = [0,0];
lf = 2;
first = 2-shift;
last = sx+lf-1;
Lo_D = [sqrt(0.5),sqrt(0.5)];
Hi_D = [-sqrt(0.5),sqrt(0.5)];

% y = [x(:,1),x,x(:,sx(2))];
% z = zeros(size(y,1),size(y,2)-1);
% for ii = 1:size(z,2)
%   z(:,ii) = y(:,ii:ii+1)*Lo_D(:);
% end
data_matlab = convdown(x,Lo_D,first,last);

function y = convdown(x,F,first,last)
% y = convdown(x,F,dwtEXTM,lenEXT,first,last)
y = x(:,first(2):2:last(2));
y = [y(1,:);y;y(size(y,1),:)];
% y = wextend('addrow',dwtEXTM,y,lenEXT); [row,col] = size(y); y1 = y'; z =
% zeros(col,row-1); for ii = 1:size(z,2)
%   z(:,ii) = y1(:,ii:ii+1)*F(:);
% end y = conv2(y',F(:)','valid'); sum((y-z)>1e-10,'all')
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