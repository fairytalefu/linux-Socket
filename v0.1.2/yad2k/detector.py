#! /usr/bin/env python
"""Run a YOLO_v2 style detection model on test images."""
import argparse
import colorsys
import imghdr
import os
import random

import numpy as np
from keras import backend as K
from keras.models import load_model
from PIL import Image, ImageDraw, ImageFont

from yad2k.models.keras_yolo import yolo_eval, yolo_head


class YOLO(object): 
	def __init__(self): 
		self.model_path = 'model_data/yolo.h5' 
		self.anchors_path = 'model_data/yolo_anchors.txt' 
		self.classes_path = 'model_data/coco_classes.txt'
		self.test_path = 'images/'
		self.output_path = 'images/out'
		self.score_threshold = 0.3 
		self.iou_threshold = 0.5 
		self.class_names = self._get_class() 
		self.anchors = self._get_anchors()
		self.num_classes = len(self.class_names)
		self.num_anchors = len(self.anchors)
		self.yolo_model = self._get_model()
		self.colors = self._get_colors()
	def _get_class(self):
		with open(self.classes_path) as f:
			class_names = f.readlines() 
		class_names = [c.strip() for c in class_names] 
		return class_names 
	def _get_anchors(self): 
		with open(self.anchors_path) as f: 
			anchors = f.readline() 
			anchors = [float(x) for x in anchors.split(',')] 
			anchors = np.array(anchors).reshape(-1, 2)
		return anchors 
	def _get_model(self):
		yolo_model = load_model(self.model_path)
		# TODO: Assumes dim ordering is channel last
		model_output_channels = yolo_model.layers[-1].output_shape[-1]
		assert model_output_channels == self.num_anchors * (self.num_classes + 5), \
		'Mismatch between model and given anchor and class sizes. ' \
		'Specify matching anchors and classes with --anchors_path and ' \
		'--classes_path flags.'
		print('{} model, anchors, and classes loaded.'.format(self.model_path))
		return yolo_model
	def _get_colors(self):
		 # Generate colors for drawing bounding boxes.
		hsv_tuples = [(x / len(self.class_names), 1., 1.) for x in range(len(self.class_names))]
		colors = list(map(lambda x: colorsys.hsv_to_rgb(*x), hsv_tuples))
		colors = list(map(lambda x: (int(x[0] * 255), int(x[1] * 255), int(x[2] * 255)),colors))
		return colors
	def detect_image(self):
		sess = K.get_session()
		# Check if model is fully convolutional, assuming channel last order.
		model_image_size = self.yolo_model.layers[0].input_shape[1:3]
		is_fixed_size = model_image_size != (None, None)
		random.seed(10101)  # Fixed seed for consistent colors across runs.
		random.shuffle(self.colors)  # Shuffle colors to decorrelate adjacent classes.
		random.seed(None)  # Reset seed to default.
		# Generate output tensor targets for filtered bounding boxes.
		# TODO: Wrap these backend operations with Keras layers.
		yolo_outputs = yolo_head(self.yolo_model.output, self.anchors, len(self.class_names))
		input_image_shape = K.placeholder(shape=(2, ))
		boxes, scores, classes = yolo_eval(yolo_outputs,input_image_shape,score_threshold=self.score_threshold,iou_threshold=self.iou_threshold)
		for image_file in os.listdir(self.test_path):
			try:
				image_type = imghdr.what(os.path.join(self.test_path, image_file))
				if not image_type:
					continue
			except IsADirectoryError:
				continue
			image = Image.open(os.path.join(self.test_path, image_file))
			if is_fixed_size:  # TODO: When resizing we can use minibatch input.
				resized_image = image.resize(tuple(reversed(model_image_size)), Image.BICUBIC)
				image_data = np.array(resized_image, dtype='float32')
			else:
				# Due to skip connection + max pooling in YOLO_v2, inputs must have
				# width and height as multiples of 32.
				new_image_size = (image.width - (image.width % 32),image.height - (image.height % 32))
				resized_image = image.resize(new_image_size, Image.BICUBIC)
				image_data = np.array(resized_image, dtype='float32')
				print(image_data.shape)

			image_data /= 255.
			image_data = np.expand_dims(image_data, 0)  # Add batch dimension.

			out_boxes, out_scores, out_classes = sess.run([boxes, scores, classes],feed_dict={self.yolo_model.input: image_data,input_image_shape: [image.size[1], image.size[0]],K.learning_phase(): 0})
			print('Found {} boxes for {}'.format(len(out_boxes), image_file))
			font = ImageFont.truetype(font='font/FiraMono-Medium.otf',size=np.floor(3e-2 * image.size[1] + 0.5).astype('int32'))
			thickness = (image.size[0] + image.size[1]) // 300
			for i, c in reversed(list(enumerate(out_classes))):
				predicted_class = self.class_names[c]
				box = out_boxes[i]
				score = out_scores[i]

				label = '{} {:.2f}'.format(predicted_class, score)

				draw = ImageDraw.Draw(image)
				label_size = draw.textsize(label, font)

				top, left, bottom, right = box
				top = max(0, np.floor(top + 0.5).astype('int32'))
				left = max(0, np.floor(left + 0.5).astype('int32'))
				bottom = min(image.size[1], np.floor(bottom + 0.5).astype('int32'))
				right = min(image.size[0], np.floor(right + 0.5).astype('int32'))
				print(label, (left, top), (right, bottom))
				if top - label_size[1] >= 0:
					text_origin = np.array([left, top - label_size[1]])
				else:
					text_origin = np.array([left, top + 1])

				# My kingdom for a good redistributable image drawing library.
				for i in range(thickness):
					draw.rectangle([left + i, top + i, right - i, bottom - i],outline=self.colors[c])
				draw.rectangle([tuple(text_origin), tuple(text_origin + label_size)],fill=self.colors[c])
				draw.text(text_origin, label, fill=(0, 0, 0), font=font)
				del draw
			image.save(os.path.join(self.output_path, image_file), quality=90)
		sess.close()

def detector():
	yolo = YOLO()
	yolo.detect_image()

if __name__ == '__main__':
    detector()
