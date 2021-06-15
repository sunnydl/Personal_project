import pygame

pygame.init()

w = 500
h = 600

screen = pygame.display.set_mode((w, h))
pygame.display.set_caption("Sudoku simulator")
icon1 = pygame.image.load('smartphone.png')
pygame.display.set_icon(icon1)

x = 0
y = 0
diff = w/9
val = 0
grid =[ 
        [7, 8, 0, 4, 0, 0, 1, 2, 0], 
        [6, 0, 0, 0, 7, 5, 0, 0, 9], 
        [0, 0, 0, 6, 0, 1, 0, 7, 8], 
        [0, 0, 7, 0, 4, 0, 2, 6, 0], 
        [0, 0, 1, 0, 5, 0, 9, 3, 0], 
        [9, 0, 4, 0, 6, 0, 0, 0, 5], 
        [0, 7, 0, 3, 0, 0, 0, 1, 2], 
        [1, 2, 0, 0, 0, 7, 4, 0, 0], 
        [0, 4, 9, 2, 0, 6, 0, 0, 7] 
    ]

font1 = pygame.font.SysFont("comicsans", 40, True)
font2 = pygame.font.SysFont("comicsans", 40, False, True)
font3 = pygame.font.SysFont("bodoni", 25, True, True)

def coordinate(pos):
    global x
    x = int(pos[0]//diff)
    global y
    y = int(pos[1]//diff)

def highlight_box():
    for i in range(2):
        pygame.draw.line(screen, (255, 255, 0), (x*diff, (y+i)*diff), ((x+1)*diff, (y+i)*diff), 7)
        pygame.draw.line(screen, (255, 255, 0), ((x+i)*diff, y*diff), ((x+i)*diff, (y+1)*diff), 7)

def draw():
    for i in range(9):
        for j in range(9):
            pygame.draw.line(screen, (0, 0, 0), (i*diff, 0), (i*diff, 500), 5)
            pygame.draw.line(screen, (0, 0, 0), (0, j*diff), (500, j*diff), 5)
            text1 = font1.render(str(grid[i][j]), True, (0, 0, 0))
            screen.blit(text1, (i*diff + 15, j*diff + 15))
    pygame.draw.line(screen, (0, 0, 0), (0, h-diff*2+10), (w, h-diff*2+10), 5)
    pygame.draw.line(screen, (0, 0, 0), (0, h-diff*2+48), (w, h-diff*2+48), 3)

def guide():
    text3 = font3.render("Press \"enter\" to automatically solve the board.", True, (0, 0 ,0))
    screen.blit(text3, (20, 540))
    text4 = font3.render("Press \"ESC\" to return to default board.", True, (0, 0, 0))
    screen.blit(text4, (20, 560))
    text5 = font3.render("Press \"c\" to go into custome mode", True, (0, 0, 0))
    screen.blit(text5, (20, 580))

def validate_board(grid):
    for i in range(9):
        for j in range(9):
            v = grid[i][j]
            if(v==0):
                continue
            if(validate(grid, i, j, v)):
                continue
            else:
                return False
    return True

def validate(grid, r, c, val):
    for i in range(9):
        if(grid[r][i]==val and (i!=c)):
            return False
        if(grid[i][c]==val and (i!=r)):
            return False
    sRow = r//3
    sCol = c//3
    for i in range(3):
        for j in range(3):
            if(grid[i+sRow*3][j+sCol*3]==val and (i+sRow*3)!=r and (j+sCol*3)!=c):
                return False
    return True


def solver(grid, r, c):
    if(r==8 and c==9):
        return True
    if(c==9):
        r = r + 1
        c = 0
    if(grid[r][c]>0):
        return solver(grid, r, c+1)
    for i in range(1, 10):
        if(validate(grid, r, c, i)):
            grid[r][c]=i
            if(solver(grid, r, c+1)):
                return 1
        grid[r][c] = 0
    return False

highlight = 0
change = 0
run = True
message = -1
while(run):
    pygame.draw.rect(screen, (192, 192, 192), (0, 0, 500, 500))
    pygame.draw.rect(screen, (255, 255, 255), (0, 500, 500, 100))

    for event in pygame.event.get():
        if(event.type == pygame.QUIT):
            run = False
        if(event.type == pygame.MOUSEBUTTONDOWN):
            pos = pygame.mouse.get_pos()
            coordinate(pos)
            highlight = 1
        if (event.type == pygame.KEYDOWN):
            change = 1
            if (event.key == pygame.K_0):
                grid[x][y]=0
            if (event.key == pygame.K_1):
                grid[x][y]=1
            if (event.key == pygame.K_2):
                grid[x][y]=2
            if (event.key == pygame.K_3):
                grid[x][y]=3
            if (event.key == pygame.K_4):
                grid[x][y]=4
            if (event.key == pygame.K_5):
                grid[x][y]=5
            if (event.key == pygame.K_6):
                grid[x][y]=6
            if (event.key == pygame.K_7):
                grid[x][y]=7
            if (event.key == pygame.K_8):
                grid[x][y]=8
            if (event.key == pygame.K_9):
                grid[x][y]=9
            if (event.key == pygame.K_RETURN):
                if(validate_board(grid)==False):
                    message = 2
                elif(solver(grid, 0, 0)):
                    message = 1
                else:
                    message = 2
            if(event.key == pygame.K_ESCAPE):
                grid =[ 
                        [7, 8, 0, 4, 0, 0, 1, 2, 0], 
                        [6, 0, 0, 0, 7, 5, 0, 0, 9], 
                        [0, 0, 0, 6, 0, 1, 0, 7, 8], 
                        [0, 0, 7, 0, 4, 0, 2, 6, 0], 
                        [0, 0, 1, 0, 5, 0, 9, 3, 0], 
                        [9, 0, 4, 0, 6, 0, 0, 0, 5], 
                        [0, 7, 0, 3, 0, 0, 0, 1, 2], 
                        [1, 2, 0, 0, 0, 7, 4, 0, 0], 
                        [0, 4, 9, 2, 0, 6, 0, 0, 7] 
                        ]
                message = 0
            if(event.key == pygame.K_c):
                grid =[ 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0] 
                        ]
                message = 3
    if(highlight==1):
        highlight_box()
    if(message==0):
        text2 = font2.render("Default board loaded", True, (0, 0, 0))
        screen.blit(text2, (20, 510))
    if(message==1):
        text2 = font2.render("Board is solved!", True, (0, 255, 0))
        screen.blit(text2, (20, 510))
    if(message==2):
        text2 = font2.render("Board is not solvable", True, (128, 0, 0))
        screen.blit(text2, (20, 510))
    if(message==3):
        text2 = font2.render("Custom mode", True, (128, 128, 128))
        screen.blit(text2, (20, 510))
    draw()
    guide()
    pygame.display.update()

=======
import pygame

pygame.init()

w = 500
h = 600

screen = pygame.display.set_mode((w, h))
pygame.display.set_caption("Sudoku simulator")
icon1 = pygame.image.load('smartphone.png')
pygame.display.set_icon(icon1)

x = 0
y = 0
diff = w/9
val = 0
grid =[ 
        [7, 8, 0, 4, 0, 0, 1, 2, 0], 
        [6, 0, 0, 0, 7, 5, 0, 0, 9], 
        [0, 0, 0, 6, 0, 1, 0, 7, 8], 
        [0, 0, 7, 0, 4, 0, 2, 6, 0], 
        [0, 0, 1, 0, 5, 0, 9, 3, 0], 
        [9, 0, 4, 0, 6, 0, 0, 0, 5], 
        [0, 7, 0, 3, 0, 0, 0, 1, 2], 
        [1, 2, 0, 0, 0, 7, 4, 0, 0], 
        [0, 4, 9, 2, 0, 6, 0, 0, 7] 
    ]

font1 = pygame.font.SysFont("comicsans", 40, True)
font2 = pygame.font.SysFont("comicsans", 40, False, True)
font3 = pygame.font.SysFont("bodoni", 25, True, True)

def coordinate(pos):
    global x
    x = int(pos[0]//diff)
    global y
    y = int(pos[1]//diff)

def highlight_box():
    for i in range(2):
        pygame.draw.line(screen, (255, 255, 0), (x*diff, (y+i)*diff), ((x+1)*diff, (y+i)*diff), 7)
        pygame.draw.line(screen, (255, 255, 0), ((x+i)*diff, y*diff), ((x+i)*diff, (y+1)*diff), 7)

def draw():
    for i in range(9):
        for j in range(9):
            pygame.draw.line(screen, (0, 0, 0), (i*diff, 0), (i*diff, 500), 5)
            pygame.draw.line(screen, (0, 0, 0), (0, j*diff), (500, j*diff), 5)
            text1 = font1.render(str(grid[i][j]), True, (0, 0, 0))
            screen.blit(text1, (i*diff + 15, j*diff + 15))
    pygame.draw.line(screen, (0, 0, 0), (0, h-diff*2+10), (w, h-diff*2+10), 5)
    pygame.draw.line(screen, (0, 0, 0), (0, h-diff*2+48), (w, h-diff*2+48), 3)

def guide():
    text3 = font3.render("Press \"enter\" to automatically solve the board.", True, (0, 0 ,0))
    screen.blit(text3, (20, 540))
    text4 = font3.render("Press \"ESC\" to return to default board.", True, (0, 0, 0))
    screen.blit(text4, (20, 560))
    text5 = font3.render("Press \"c\" to go into custome mode", True, (0, 0, 0))
    screen.blit(text5, (20, 580))

def validate_board(grid):
    for i in range(9):
        for j in range(9):
            v = grid[i][j]
            if(v==0):
                continue
            if(validate(grid, i, j, v)):
                continue
            else:
                return False
    return True

def validate(grid, r, c, val):
    for i in range(9):
        if(grid[r][i]==val and (i!=c)):
            return False
        if(grid[i][c]==val and (i!=r)):
            return False
    sRow = r//3
    sCol = c//3
    for i in range(3):
        for j in range(3):
            if(grid[i+sRow*3][j+sCol*3]==val and (i+sRow*3)!=r and (j+sCol*3)!=c):
                return False
    return True


def solver(grid, r, c):
    if(r==8 and c==9):
        return True
    if(c==9):
        r = r + 1
        c = 0
    if(grid[r][c]>0):
        return solver(grid, r, c+1)
    for i in range(1, 10):
        if(validate(grid, r, c, i)):
            grid[r][c]=i
            if(solver(grid, r, c+1)):
                return 1
        grid[r][c] = 0
    return False

highlight = 0
change = 0
run = True
message = -1
while(run):
    pygame.draw.rect(screen, (192, 192, 192), (0, 0, 500, 500))
    pygame.draw.rect(screen, (255, 255, 255), (0, 500, 500, 100))

    for event in pygame.event.get():
        if(event.type == pygame.QUIT):
            run = False
        if(event.type == pygame.MOUSEBUTTONDOWN):
            pos = pygame.mouse.get_pos()
            coordinate(pos)
            highlight = 1
        if (event.type == pygame.KEYDOWN):
            change = 1
            if (event.key == pygame.K_0):
                grid[x][y]=0
            if (event.key == pygame.K_1):
                grid[x][y]=1
            if (event.key == pygame.K_2):
                grid[x][y]=2
            if (event.key == pygame.K_3):
                grid[x][y]=3
            if (event.key == pygame.K_4):
                grid[x][y]=4
            if (event.key == pygame.K_5):
                grid[x][y]=5
            if (event.key == pygame.K_6):
                grid[x][y]=6
            if (event.key == pygame.K_7):
                grid[x][y]=7
            if (event.key == pygame.K_8):
                grid[x][y]=8
            if (event.key == pygame.K_9):
                grid[x][y]=9
            if (event.key == pygame.K_RETURN):
                if(validate_board(grid)==False):
                    message = 2
                elif(solver(grid, 0, 0)):
                    message = 1
                else:
                    message = 2
            if(event.key == pygame.K_ESCAPE):
                grid =[ 
                        [7, 8, 0, 4, 0, 0, 1, 2, 0], 
                        [6, 0, 0, 0, 7, 5, 0, 0, 9], 
                        [0, 0, 0, 6, 0, 1, 0, 7, 8], 
                        [0, 0, 7, 0, 4, 0, 2, 6, 0], 
                        [0, 0, 1, 0, 5, 0, 9, 3, 0], 
                        [9, 0, 4, 0, 6, 0, 0, 0, 5], 
                        [0, 7, 0, 3, 0, 0, 0, 1, 2], 
                        [1, 2, 0, 0, 0, 7, 4, 0, 0], 
                        [0, 4, 9, 2, 0, 6, 0, 0, 7] 
                        ]
                message = 0
            if(event.key == pygame.K_c):
                grid =[ 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0], 
                        [0, 0, 0, 0, 0, 0, 0, 0, 0] 
                        ]
                message = 3
    if(highlight==1):
        highlight_box()
    if(message==0):
        text2 = font2.render("Default board loaded", True, (0, 0, 0))
        screen.blit(text2, (20, 510))
    if(message==1):
        text2 = font2.render("Board is solved!", True, (0, 255, 0))
        screen.blit(text2, (20, 510))
    if(message==2):
        text2 = font2.render("Board is not solvable", True, (128, 0, 0))
        screen.blit(text2, (20, 510))
    if(message==3):
        text2 = font2.render("Custom mode", True, (128, 128, 128))
        screen.blit(text2, (20, 510))
    draw()
    guide()
    pygame.display.update()

pygame.quit()