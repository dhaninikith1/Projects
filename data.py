from keras import layers
from keras.layers import Input, Dense, Activation, ZeroPadding2D, BatchNormalization, Flatten, Conv2D
from keras.layers import AveragePooling2D, MaxPooling2D, Dropout, GlobalMaxPooling2D, GlobalAveragePooling2D
from keras.models import Sequential
from keras.preprocessing import image
from keras.utils import layer_utils
from keras.optimizers import SGD
from keras.models import load_model
import numpy as np
import os
from keras import backend as K

def main():
    # clear kaggles cache
    K.clear_session()

    # Change this based upon what split size of the data we are doing
    split_size = 3

    amount_of_image_splits = 30 / split_size # images / split#

    # Load in pre-trained model for music classification
    model = load_model("models/best_model_128_480epochs")
    #model = load_model("models/rand_save")

    # load in testing data
    X_test = np.load("X_mel128_test_3.dat")
    # reshape so in form for CNN-Keras, only need to do for 1 channel images (black and white)
    #X_test = X_test.reshape(X_test.shape[0], 174, 124, 3)
    print(X_test.shape)

    # normalize testing data
    X_test = X_test / 255

    # make predictions on validation data from Kaggle
    predictions = model.predict_classes(X_test, verbose=1)
    print(predictions)

    # Converting predictions to genre names
    prediction_names = convert_numeric_to_words(predictions)

    # take ensemble of split images to classify one image, taking in AMOUNT_OF_IMAGE_SPLITS # of votes
    final_predictions = convert_predictions_to_votes(prediction_names, amount_of_image_splits)
    print(final_predictions)

    # save predictions to output file in format for Kaggle submission
    save_predictions(final_predictions)

def convert_numeric_to_words(predictions):
    prediction_names = []

    for prediction in predictions:
        if prediction == 0:
            prediction_names.append("blues")
        elif prediction == 1:
            prediction_names.append("classical")
        elif prediction == 2:
            prediction_names.append("country")
        elif prediction == 3:
            prediction_names.append("disco")
        elif prediction == 4:
            prediction_names.append("hiphop")
        elif prediction == 5:
            prediction_names.append("jazz")
        elif prediction == 6:
            prediction_names.append("metal")
        elif prediction == 7:
            prediction_names.append("pop")
        elif prediction == 8:
            prediction_names.append("reggae")
        else:
            prediction_names.append("rock")

    return prediction_names

def convert_predictions_to_votes(prediction_names, amount_of_image_splits):
    print(len(prediction_names))
    # convert predictions to votes
    final_predictions = []

    keys = {"blues", "classical", "country", "disco", "hiphop", "jazz", "metal", "pop", "reggae", "rock"}
    current_song_vote = {key: 0 for key in keys}
    print(current_song_vote)

    counter = 0
    for prediction in prediction_names:
        if (counter % amount_of_image_splits == 0) and counter != 0:
            # get top vote
            top_vote = max(current_song_vote, key=current_song_vote.get)
            # add vote to list of final predictioons
            final_predictions.append(top_vote)
            # reset current votes
            current_song_vote = {key: 0 for key in keys}
            counter = 0

        counter += 1
        current_song_vote[prediction] += 1

    # get the last set of votes (still 10 votes, but since we don't go to next one, it never gets grabbed)
    top_vote = max(current_song_vote, key=current_song_vote.get)
    final_predictions.append(top_vote)
    print("Final prediction length " + str(len(final_predictions)))

    return final_predictions

def save_predictions(final_predictions):
    # can just use original names
    file_saving_to = "output.csv"
    list_of_file_names = []
    for file in os.listdir(os.getcwd() + "/validation"):
        list_of_file_names.append(file)

    output_file = open(file_saving_to, "w")
    output_file.write("id,class\n")
    i = 0
    for prediction in final_predictions:
        output_file.write("%s,%s\n" % (list_of_file_names[i], prediction))
        i += 1

    output_file.close()
    print("Successfully saved predictions to: " + file_saving_to)

if __name__ == "__main__":
    main()