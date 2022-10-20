Module.onRuntimeInitialized = () => { start(0,1); }

// Demo Canvas (JavaScript)

/* ******************** global variables ******************** */

var canvas = document.getElementById('mycanvas');
const LIGHTBULB = 1;

canvas.addEventListener('click', canvasLeftClick);        // left click event

/************************************************************************/

function canvasLeftClick(event) {
    event.preventDefault(); // prevent default context menu to appear...
    // get relative cursor position in canvas
    console.log("left click at position:", event.offsetX, event.offsetY);
    // update position of mario image used by drawCanvas()
    drawCanvas(event.offsetX, event.offsetY);
}
/*************************************************************/

function printGame(g,ctx) {
    var text = "";
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    var w = canvas.width;
    var h = canvas.height;
    var p = w/7;
    var x=0;
    for (var row = 0; row < nb_rows; row++) {
        x+=1;
        y=0;
        for (var col = 0; col < nb_cols; col++) {
            y+=1;
            var blank = Module._is_blank(g, row, col);
            var black = Module._is_black(g, row, col);
            var ligthed = Module._is_lighted(g, row, col);
            var lightbulb = Module._is_lightbulb(g, row, col);
            var marked = Module._is_marked(g, row, col);
            var error = Module._has_error(g, row, col);
            if (black){
                ctx.save();
                ctx.fillStyle = 'black';
                ctx.fillRect(p*(x-1), p*(y-1), p, p);
                ctx.restore();
                var b_val = Module._get_black_number(g,row,col);
                if (b_val==0 || b_val==1 || b_val==2 || b_val==3 || b_val==4){
                    ctx.save();
                    ctx.font = 'bold 50px Arial';
                    ctx.fillStyle = 'white';
                    ctx.fillText(b_val, ((x-1)*(p))+15, ((y-0.20)*(p)));
                    ctx.restore();}
            }
            if (blank){
                ctx.save();
                ctx.fillStyle = 'lightgray';
                ctx.fillRect(p*(x-1), p*(y-1), p, p);
                ctx.restore();}
            if (ligthed){
                ctx.save();
                ctx.fillStyle = 'yellow';
                ctx.fillRect(p*(x-1), p*(y-1), p, p);
                ctx.restore();}
            if (lightbulb){
                ctx.save();
                ctx.fillStyle = 'white';
                ctx.beginPath();
                ctx.arc(p*(x-1)+p/2, p*(y-1)+p/2, p/3, 0,2*Math.PI);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();}
            if (error){
                ctx.save();
                ctx.fillStyle = 'rgba(255,0,0,0.8)';
                ctx.fillRect(p*(x-1), p*(y-1), p, p);
                ctx.restore();}
            if (marked){
                ctx.save();
                ctx.fillStyle = 'black';
                ctx.fillRect((p*(x-1))+0.5*p, p*(y-1)+0.5*p, p/4, p);
                ctx.restore();}
        }
        text += "\n";
    }
    drawLine (ctx,canvas.height,canvas.width);
}

function remouv(g,ctx,x,y){
    if (x!= undefined && y!= undefined){
        Module._play_move(g, x/57 , y/57, 0);
    }
    printGame(g,ctx);
}

function play_mouv(g,ctx,x,y){
    if (x!= undefined && y!= undefined){
        Module._play_move(g, x/57 , y/57, LIGHTBULB);
    }
    console.log("je met tout sur le screen");
    printGame(g,ctx);
}

function start(solve,restart,undo,redo) {
    console.log("ze barti");
    if (restart == 1){
        g = Module._new_default();
    }
    if(solve==1){
        g = Module._new_default();
        Module._solve(g);
    }
    if(undo==1){
        Module._undo(g);
    }
    if(redo==1){
        Module._redo(g);
    }
    drawCanvas();
}

function quit() {
    alert("Vous ne pouvez pas quitter le jeu !!!");
  }

function undo(g){
    start(0,0,1);
}

function redo(g){
    start(0,0,0,1);
}

function restart(g){
    start(0,1);
}

function solve(g){
    start(1);
}
/* ******************** canvas drawing ******************** */

function drawLine (ctx,height,width){
    ctx.save();
    ctx.strokeStyle = 'gray';
    var p = height/7;
    // draw some lines
    for (var i = 0; i < 7; i += 1) {
        ctx.moveTo(0, p*i);
        ctx.lineTo(width,p*i);
        ctx.moveTo(p*i,0);
        ctx.lineTo(p*i,height);
    }
    ctx.stroke(); // the stroke will actually paint the current lines
    ctx.restore();
}

function drawCanvas(x,y) {
    var ctx = canvas.getContext('2d');
    var width = canvas.width;
    var height = canvas.height;

    // clear canvas
    ctx.clearRect(0, 0, width, height);

    drawLine(ctx,height,width);
    play_mouv(g,ctx);
    if (x!= undefined && y!= undefined){
        if (Module._is_lightbulb(g, x/57 , y/57) == true){
            remouv(g,ctx,x,y);
        }else {
        play_mouv(g,ctx,x,y);
        }
    }

}

// EOF

