--> With Love2d
local PointObject;
PointObject = {
    new = function(MaxPoints,Title)
        return setmetatable({
            Title = Title or "Untitled Dataset",
            Max_Points = MaxPoints or 100,
            CurrentX = 0,
            Highest_Point = 0,
            Total = 0,
            Average = 0,
            Color = {
                R = 0,
                G = 1,
                B = 0,
                A = 1,
            },
            Points = {},
        },PointObject);
    end,
};
PointObject.__index = PointObject;

function PointObject:DrawColor(A)
    love.graphics.setColor(self.Color.R,self.Color.G,self.Color.B,A or self.Color.A);
end

function PointObject:SetColor(R,G,B,A)
    self.Color = {
        R = R or 0,
        G = G or 0,
        B = B or 0,
        A = A or 1,
    };
end

function PointObject:GetMax()
    local CurrentMax = 0;
    for _, Variable in pairs(self.Points) do 
        CurrentMax = math.max(CurrentMax, Variable);
    end
    return CurrentMax;
end

function PointObject:GetMin()
    local LowestPoint = self.Highest_Point;
    for _, Variable in pairs(self.Points) do 
        LowestPoint = math.min(LowestPoint, Variable);
    end
    return LowestPoint;
end

function PointObject:Clear()
    self.Highest_Point = 0;
    self.Points = {};
    self.CurrentX = 0;
end

function PointObject:SetMaxPoints(Value)
    self.Max_Points = Value;
end

function PointObject:Add_Point(Value)
    self.Total = self.Total + Value;
    self.CurrentX = math.min(self.CurrentX+1,self.Max_Points);
    if self.CurrentX == self.Max_Points then 
        self.Total = self.Total-self.Points[1];
        table.remove(self.Points,1);

        local CurrentMax = self:GetMax();
        if CurrentMax < self.Highest_Point then 
            self.Highest_Point = math.max(CurrentMax,self.Highest_Point - self.Highest_Point*0.01);
        end

        self.Average = self.Total/self.Max_Points;
    end
    self.Highest_Point = math.max(self.Highest_Point, Value);
    table.insert(self.Points,Value);
end

local Graph;
Graph = {
    PointSet = PointObject,
    new = function(Position, Size, Title)
        return setmetatable({
            Marker = true,
            Grid = true,
            Scale = false,
            Position = Position or Vector2.new(0,0),
            Size = Size or Vector2.new(10,10),
            Title = Title or "Untitled Graph",
        },Graph);
    end,
};
Graph.__index = Graph;

function Graph:Resize(Size)
    self.Size = Size;
end

function Graph:Move(Position)
    self.Position = Position;
end

function Graph:Draw(...)

    local Points = {...};

    local Highest = 0;
    local Length = 0;
    local KeyLength = 1;

    for _,v in pairs(Points) do
        KeyLength = math.max(KeyLength,__Basic_Font:getWidth(v.Title .. ", Average:\t" .. tostring(math.floor(v.Average*100000)/100000)));
        Highest = math.max(v.Highest_Point,Highest);
        Length = math.max(v.Max_Points,Length);
    end

    local X_Scale = (self.Size.X/Length);
    local Y_Scale = (self.Size.Y/Highest);

    if self.Marker then

        local __Size = __Graphing_Font:getWidth(tostring((math.floor(Highest*100000)/100000)))+15;

        love.graphics.setColor(0.8,0.8,0.8,1);
        love.graphics.rectangle('fill',self.Position.X-__Size,self.Position.Y,self.Size.X+__Size,self.Size.Y+15);

        love.graphics.setColor(0.5,0.5,0.5,0.5);
        love.graphics.rectangle('line',self.Position.X-__Size,self.Position.Y,self.Size.X+__Size,self.Size.Y+15);

        love.graphics.setColor(0,0,0,1);
        love.graphics.setFont(__Graphing_Font);

        for i=0,10,1 do 
            local Number = tostring(math.floor(Length*(i/10)));
            love.graphics.print(Number, self.Position.X + (Length*(i/10))*X_Scale - __Graphing_Font:getWidth(Number)-2, self.Position.Y+self.Size.Y+2);
        end

        for i=0,10,1 do 
            local Number = tostring(math.floor(Highest*(i/10)*100000)/100000);
            love.graphics.print(Number, self.Position.X-__Graphing_Font:getWidth(Number)-2, self.Position.Y + self.Size.Y - self.Size.Y*(i/10)+2);
        end

        love.graphics.setFont(__Basic_Font);

    end

    love.graphics.setColor(1,1,1,1);
    love.graphics.rectangle("fill",self.Position.X,self.Position.Y,self.Size.X,self.Size.Y);

    for _, Point in pairs(Points) do 


        local X_Scale = self.Scale and self.Size.X/Point.Max_Points or X_Scale;
        local Y_Scale = self.Scale and self.Size.Y/Point.Highest_Point or Y_Scale;

        Point:DrawColor();
        for i=1,#Point.Points,1 do 

            local NotMax = (i < #Point.Points);

            local Y = Point.Points[i];
            local nY = NotMax and Point.Points[i+1] or Y;

            love.graphics.line(self.Position.X + i*X_Scale, self.Position.Y+self.Size.Y - Y*Y_Scale, self.Position.X + (i + (NotMax and 1 or 0))*X_Scale, self.Position.Y+self.Size.Y - nY*Y_Scale);

        end
    
    end

    if self.Grid then 

        love.graphics.setColor(0.5,0.5,0.5,0.8);

        for i=0,10,1 do 
            love.graphics.line(self.Position.X + (Length*(i/10))*X_Scale, self.Position.Y+self.Size.Y, self.Position.X + (Length*(i/10))*X_Scale, self.Position.Y);
        end

        for i=0,10,1 do 
            love.graphics.line(self.Position.X, self.Position.Y + self.Size.Y*(i/10), self.Position.X + self.Size.X, self.Position.Y + self.Size.Y*(i/10));
        end

    end

    love.graphics.setColor(0,0,0,0.5);
    love.graphics.rectangle("fill",self.Position.X,self.Position.Y,__Basic_Font:getWidth(self.Title)+6,20);
    
    love.graphics.setColor(1,1,1,0.5);
    love.graphics.rectangle("fill",self.Position.X+self.Size.X,self.Position.Y,KeyLength+4,12*#Points+8);

    love.graphics.setColor(0.2,0.2,0.2,1);
    love.graphics.rectangle("line",self.Position.X,self.Position.Y,__Basic_Font:getWidth(self.Title)+6,20);
    love.graphics.rectangle("line",self.Position.X+self.Size.X,self.Position.Y,KeyLength+4,12*#Points+8);

    for _,Point in pairs(Points) do 
        Point:DrawColor(1);
        love.graphics.print(Point.Title .. ", Average:\t" .. tostring(math.floor(Point.Average*100000)/100000), self.Position.X+self.Size.X+2, self.Position.Y + 2 + (_-1)*12);
    end


    love.graphics.setColor(0.5,0.5,0.5,1);
    love.graphics.rectangle("line",self.Position.X,self.Position.Y,self.Size.X,self.Size.Y);
    
    love.graphics.setColor(1,1,1,1);
    love.graphics.print(self.Title,self.Position.X+2,self.Position.Y+2);

end

return Graph;
