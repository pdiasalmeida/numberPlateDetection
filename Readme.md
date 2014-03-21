# Detetor de matrículas

Este projeto tenta detetar uma matrícula em duas fases:
* detetar a área da matrícula
* detetar as letras e números

Para a detecção das possiveis áreas de matrícula apresenta duas alternativas: baseada na forma geométrica da matrícula; ou baseada nas edges verticais que estas apresentam.
Para a identificação das letras e números é utilizada a biblioteca Tesseract OCR.

Este problema também é abordado no capítulo 5 do livro *Mastering OpenCV with Practical Computer Vision Projects* que tem o seguinte repositório associado: https://github.com/MasteringOpenCV/code/tree/master/Chapter5_NumberPlateRecognition .
Foi deste repositório que foram obtidas as imagens de teste e donde se baseou o método de detecção baseado nas edges verticais.

## Dependências

* OpenCV: http://opencv.org/downloads.html;
* Tesseract OCR: https://code.google.com/p/tesseract-ocr. 